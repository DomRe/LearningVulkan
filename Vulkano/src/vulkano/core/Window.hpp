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
			int width         = 0;
			int height        = 0;
			bool enable_debug = false;
			std::string title = "";
		};

		Window(const WindowSettings& window_settings, const VkApplicationInfo& vulkan_settings);
		~Window();

		void poll_events();

	private:
		Window() = delete;

		[[nodiscard]] bool valid_device(VkPhysicalDevice device);

	private:
		bool m_debug_mode;
		GLFWwindow* m_window;
		VkInstance m_vk_instance;
		VkDebugUtilsMessengerEXT m_debug_messenger;
		VkPhysicalDevice m_gpu;
	};
} // namespace vulkano

#endif