#ifndef VULKANO_WINDOW_HPP_
#define VULKANO_WINDOW_HPP_

#include <string_view>

#include <GLFW/glfw3.h>

#include "vulkano/utils/Utility.hpp"

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

		[[nodiscard]] const bool is_open();
		void close();

	private:
		Window() = delete;

		[[nodiscard]] QueueFamilyIndexs get_family_indexs(VkPhysicalDevice device);
		[[nodiscard]] const bool valid_device(VkPhysicalDevice device);

	private:
		bool m_debug_mode;
		GLFWwindow* m_window;
		VkInstance m_vk_instance;
		VkDebugUtilsMessengerEXT m_debug_messenger;
		VkPhysicalDevice m_gpu;
		VkDevice m_gpu_interface;
		VkQueue m_graphics_queue;
	};
} // namespace vulkano

#endif