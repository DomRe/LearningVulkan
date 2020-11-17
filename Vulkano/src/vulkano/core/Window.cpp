#include <array>

#include "vulkano/utils/Log.hpp"

#include "Window.hpp"

namespace vulkano
{
	Window::Window(const WindowSettings& window_settings, const VkApplicationInfo& vulkan_settings)
	    : m_debug_mode {false}, m_window {nullptr}, m_vk_instance {nullptr}, m_debug_messenger {nullptr}, m_gpu {nullptr}
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
		    .sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		    .pApplicationInfo = &vulkan_settings
		};

		if (m_debug_mode)
		{
			const constexpr std::array<const char*, 1> layer_names =
			{
			    "VK_LAYER_KHRONOS_validation"
			};

			info.enabledLayerCount       = static_cast<std::uint32_t>(1);
			info.ppEnabledExtensionNames = layer_names.data();

			VkDebugUtilsMessengerCreateInfoEXT debug_instance_info
			{
				.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
				.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32
				{
					VK_LOG(VK_NO_THROW, "{0}.", pCallbackData->pMessage);
					return VK_SUCCESS;
				},
				.pUserData = nullptr
			};
			info.pNext = dynamic_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_instance_info);

			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		else
		{
			info.enabledLayerCount       = 0;
			info.ppEnabledExtensionNames = nullptr;
			info.pNext = nullptr;
		}

		info.enabledExtensionCount   = static_cast<std::uint32_t>(extensions.size());
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
				    .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				    .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
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

				for (const auto& device : device_list)
				{
					if (valid_device(device))
					{
						m_gpu = device;
						break;
					}
				}

				if (!m_gpu)
				{
					VK_LOG(VK_THROW, "Failed to find a valid GPU.");
				}
			}
		}
	}

	Window::~Window()
	{
		if (m_debug_mode)
		{
			auto destroy_debug_messenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_vk_instance, "vkDestroyDebugUtilsMessengerEXT"));
			if (destroy_debug_messenger != nullptr)
			{
				destroy_debug_messenger(m_vk_instance, m_debug_messenger, nullptr);
			}
		}

		vkDestroyInstance(m_vk_instance, nullptr);
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Window::poll_events()
	{
		if (!glfwWindowShouldClose(m_window))
		{
			glfwPollEvents();
		}
	}

	bool Window::valid_device(VkPhysicalDevice device)
	{
		bool result = true;

		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceProperties(device, &device_properties);
		vkGetPhysicalDeviceFeatures(device, &device_features);

		// Cannot be integrated GPU.
		if (device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			result = false;
		}

		return result;
	}
} // namespace vulkano