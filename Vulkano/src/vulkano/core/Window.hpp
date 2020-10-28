#ifndef VULKANO_WINDOW_HPP_
#define VULKANO_WINDOW_HPP_

#include <string_view>

#include <GLFW/glfw3.h>

namespace vulkano
{
	class Window final
	{
	public:
		struct WindowSettings
		{
			int width                   = 0;
			int height                  = 0;
			std::uint32_t global_layers = 0;
			std::string title           = "";
		};

		Window(const WindowSettings& window_settings, const VkApplicationInfo& vulkan_settings);
		~Window();

		void poll_events();

	private:
		Window() = delete;

	private:
		GLFWwindow* m_window;
		VkInstance m_vkinstance;
	};
} // namespace vulkano

#endif