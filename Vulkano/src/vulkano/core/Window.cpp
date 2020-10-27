#include <stdexcept>

#include "Window.hpp"

namespace vulkano
{
	Window::Window(const WindowSettings& window_settings, const VkApplicationInfo& vulkan_settings)
	{
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to load glfw.");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(window_settings.width, window_settings.height, window_settings.title.c_str(), nullptr, nullptr);
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