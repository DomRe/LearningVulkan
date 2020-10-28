#include <stdexcept>

#include "Window.hpp"

namespace vulkano
{
	Window::Window(const WindowSettings& window_settings, const VkApplicationInfo& vulkan_settings)
	    : m_window {nullptr}, m_vkinstance {nullptr}
	{
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to load glfw.");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(window_settings.width, window_settings.height, window_settings.title.c_str(), nullptr, nullptr);

		std::uint32_t count = 0;
		VkInstanceCreateInfo info {};

		info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		info.pApplicationInfo        = &vulkan_settings;
		info.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&count);
		info.enabledExtensionCount   = count;
		info.enabledLayerCount       = window_settings.global_layers;

		if (vkCreateInstance(&info, nullptr, &m_vkinstance) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create vulkan instance.");
		}
	}

	Window::~Window()
	{
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
} // namespace vulkano