#include <array>

#include "vulkano/utils/Log.hpp"

#include "Window.hpp"

namespace vulkano
{
	Window::Window(const WindowSettings& window_settings, const VkApplicationInfo& vulkan_settings)
	    : m_window {nullptr}
	{
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
		Instance::Settings instance_settings
		{
		    .m_window = m_window,
			.m_settings = vulkan_settings,
			.m_debug_mode = window_settings.enable_debug,
			.m_extensions = &extensions
		};
		// clang-format on

		m_instance = std::make_shared<Instance>(instance_settings);

		int w = 0, h = 0;
		glfwGetFramebufferSize(m_window, &w, &h);
		m_swapchain = std::make_shared<SwapChain>(m_instance.get(), glm::vec2 {w, h});
	}

	Window::~Window()
	{
		m_swapchain.reset();
		m_swapchain = nullptr;

		m_instance.reset();
		m_instance = nullptr;

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
} // namespace vulkano