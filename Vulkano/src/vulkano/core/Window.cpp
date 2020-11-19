#include <array>

#include "vulkano/utils/Log.hpp"

#include "Window.hpp"

namespace vulkano
{
	Window::Window(const WindowSettings& window_settings, const VkApplicationInfo& vulkan_settings)
	    : m_debug_mode {false}, m_window {nullptr}, m_vk_instance {nullptr}, m_debug_messenger {nullptr}, m_gpu {nullptr}, m_gpu_interface {nullptr}, m_graphics_queue {nullptr}, m_surface {nullptr}, m_surface_queue {nullptr}
	{
		m_debug_mode = window_settings.enable_debug;

		if (!glfwInit())
		{
			throw std::runtime_error("Failed to load glfw.");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(window_settings.width, window_settings.height, window_settings.title.c_str(), nullptr, nullptr);

		std::uint32_t extension_count = 0;
		auto glfw_extensions          = glfwGetRequiredInstanceExtensions(&extension_count);
		std::vector<const char*> extensions {glfw_extensions, glfw_extensions + extension_count};

		// clang-format off
		VkInstanceCreateInfo info
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.flags = VK_NULL_HANDLE,
			.pApplicationInfo = &vulkan_settings
		};

		const constexpr std::array<const char*, 1> layer_names =
		{
			"VK_LAYER_KHRONOS_validation"
		};

		if (m_debug_mode)
		{
			info.enabledLayerCount = static_cast<std::uint32_t>(layer_names.size());
			info.ppEnabledLayerNames = layer_names.data();

			VkDebugUtilsMessengerCreateInfoEXT debug_instance_info
			{
				.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				.pNext = nullptr,
				.flags = VK_NULL_HANDLE,
				.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
				.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32
				{
					VK_LOG(VK_NO_THROW, "{0}.", pCallbackData->pMessage);
					return VK_SUCCESS;
				},
				.pUserData = nullptr
			};
			info.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_instance_info);

			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		else
		{
			info.enabledLayerCount = VK_NULL_HANDLE;
			info.ppEnabledExtensionNames = nullptr;
			info.pNext = nullptr;
		}

		info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
		info.ppEnabledExtensionNames = extensions.data();

		if (vkCreateInstance(&info, nullptr, &m_vk_instance) != VK_SUCCESS)
		{
			VK_LOG(VK_THROW, "Failed to create instance.");
		}
		else
		{
			if (m_debug_mode)
			{
				VkDebugUtilsMessengerCreateInfoEXT debug_info
				{
					.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
					.pNext = nullptr,
					.flags = 0,
					.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
					.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
					.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32
					{
						VK_LOG(VK_NO_THROW, "{0}.", pCallbackData->pMessage);
						return VK_SUCCESS;
					},
					.pUserData = nullptr
				};
				// clang-format on

				auto create_debug_messenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_vk_instance, "vkCreateDebugUtilsMessengerEXT"));
				if (create_debug_messenger(m_vk_instance, &debug_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
				{
					VK_LOG(VK_THROW, "Could not create vulkan debug messenger.");
				}
			}

			if (glfwCreateWindowSurface(m_vk_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
			{
				VK_LOG(VK_THROW, "GLFW failed to create vulkan window surface.");
			}
			else
			{
				std::uint32_t device_count = 0;
				vkEnumeratePhysicalDevices(m_vk_instance, &device_count, nullptr);

				if (device_count == 0)
				{
					VK_LOG(VK_THROW, "Failed to find a GPU that supports vulkan.");
				}
				else
				{
					std::vector<VkPhysicalDevice> device_list(device_count);
					vkEnumeratePhysicalDevices(m_vk_instance, &device_count, device_list.data());

					// Required Extensions.
					// clang-format off
					std::array<const char*, 1> req_extensions =
					{
						VK_KHR_SWAPCHAIN_EXTENSION_NAME
					};
					// clang-format on

					for (const auto& device : device_list)
					{
						if (valid_device(device, req_extensions))
						{
							m_gpu = device;
							break;
						}
					}

					if (!m_gpu)
					{
						VK_LOG(VK_THROW, "Failed to find a valid GPU.");
					}
					else
					{
						auto qfi = get_family_indexs(m_gpu);

						// clang-format off
						const constexpr float priority = 1.0f;
						const std::array<VkDeviceQueueCreateInfo, 2> queue_infos =
						{
							VkDeviceQueueCreateInfo
							{
								.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
								.pNext = nullptr,
								.flags = VK_NULL_HANDLE,
								.queueFamilyIndex = qfi.m_graphics.value(),
								.queueCount = 1,
								.pQueuePriorities = &priority
							},
							VkDeviceQueueCreateInfo
							{
								.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
								.pNext = nullptr,
								.flags = VK_NULL_HANDLE,
								.queueFamilyIndex = qfi.m_present_to_surface.value(),
								.queueCount = 1,
								.pQueuePriorities = &priority
							}
						};

						// Layers are depreciated in Vulkan 1.2 for VkDeviceCreateInfo.
						VkPhysicalDeviceFeatures gpu_features{};
						VkDeviceCreateInfo gpu_device_info
						{
							.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
							.pNext = nullptr,
							.flags = VK_NULL_HANDLE,
							.queueCreateInfoCount = static_cast<std::uint32_t>(queue_infos.size()),
							.pQueueCreateInfos = queue_infos.data(),
							.enabledExtensionCount = static_cast<std::uint32_t>(req_extensions.size()),
							.ppEnabledExtensionNames = req_extensions.data(),
							.pEnabledFeatures = &gpu_features
						};

						if (m_debug_mode)
						{
							gpu_device_info.enabledLayerCount = static_cast<std::uint32_t>(layer_names.size());
							gpu_device_info.ppEnabledLayerNames = layer_names.data();
						}
						else
						{
							gpu_device_info.enabledLayerCount = VK_NULL_HANDLE;
							gpu_device_info.ppEnabledLayerNames = nullptr;
						}

						if (vkCreateDevice(m_gpu, &gpu_device_info, nullptr, &m_gpu_interface) != VK_SUCCESS)
						{
							VK_LOG(VK_THROW, "Failed to create GPU logical device.");
						}
						else
						{
							vkGetDeviceQueue(m_gpu_interface, qfi.m_graphics.value(), 0, &m_graphics_queue);
							vkGetDeviceQueue(m_gpu_interface, qfi.m_present_to_surface.value(), 0, &m_surface_queue);
						}
					}
				}
			}
		}
	}

	Window::~Window()
	{
		vkDestroyDevice(m_gpu_interface, nullptr);

		if (m_debug_mode)
		{
			auto destroy_debug_messenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_vk_instance, "vkDestroyDebugUtilsMessengerEXT"));
			if (destroy_debug_messenger != nullptr)
			{
				destroy_debug_messenger(m_vk_instance, m_debug_messenger, nullptr);
			}
		}

		vkDestroySurfaceKHR(m_vk_instance, m_surface, nullptr);
		vkDestroyInstance(m_vk_instance, nullptr);
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Window::poll_events()
	{
		glfwPollEvents();
	}

	const bool Window::is_open()
	{
		return !glfwWindowShouldClose(m_window);
	}

	void Window::close()
	{
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
	}

	QueueFamilyIndexs Window::get_family_indexs(VkPhysicalDevice device)
	{
		std::uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

		std::uint32_t index = 0;
		QueueFamilyIndexs qfi;
		for (const auto& queue_family : queue_families)
		{
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				qfi.m_graphics = std::make_optional(index);
			}

			VkBool32 surface_present_supported = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, index, m_surface, &surface_present_supported);

			if (surface_present_supported)
			{
				qfi.m_present_to_surface = index;
			}

			if (qfi.has_all_required())
			{
				break;
			}

			index++;
		}

		return std::move(qfi);
	}

	const bool Window::valid_device(VkPhysicalDevice device, std::span<const char*> req_extensions)
	{
		bool result = true;

		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceProperties(device, &device_properties);
		vkGetPhysicalDeviceFeatures(device, &device_features);

		result = get_family_indexs(device).has_all_required();

		// Cannot be integrated GPU.
		if (device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			result = false;
		}

		std::uint32_t extension_count;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> found_extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, found_extensions.data());

		for (const char* req_extension : req_extensions)
		{
			auto found = std::find_if(found_extensions.begin(), found_extensions.end(), [&](const auto& extension)
			{
					return std::string {extension.extensionName} == std::string {req_extension};
			});

			if (found == found_extensions.end())
			{
				result = false;
				break; // Can break early since all extensions are required.
			}
		}

		// If everything is still good for the device.
		if (result)
		{
			auto swap_chain_info = query_swap_chain(device);
			result = (!swap_chain_info.formats.empty()) && (!swap_chain_info.present_modes.empty());
		}


		return result;
	}

	Window::SwapChainInfo Window::query_swap_chain(VkPhysicalDevice device)
	{
		Window::SwapChainInfo info;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &info.capabilities);

		std::uint32_t surface_format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &surface_format_count, nullptr);

		if (surface_format_count != 0)
		{
			info.formats.resize(surface_format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &surface_format_count, info.formats.data());
		}

		std::uint32_t presentation_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentation_mode_count, nullptr);

		if (presentation_mode_count != 0)
		{
			info.present_modes.resize(presentation_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentation_mode_count, info.present_modes.data());
		}

		return std::move(info);
	}
} // namespace vulkano