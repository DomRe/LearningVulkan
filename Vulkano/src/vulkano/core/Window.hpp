#ifndef VULKANO_WINDOW_HPP_
#define VULKANO_WINDOW_HPP_

#include <string_view>
#include <span>
#include <vector>

#include <GLFW/glfw3.h>

#include "vulkano/utils/Utility.hpp"

namespace vulkano
{
	class Window final
	{
	public:
		///
		/// Simplifys passing around window settings.
		///
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
		///
		/// Stores information about SwapChain support that can be easily passed around.
		///
		struct SwapChainInfo
		{
			VkSurfaceCapabilitiesKHR capabilities;

			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> present_modes;
		};

		Window() = delete;

		[[nodiscard]] QueueFamilyIndexs get_family_indexs(VkPhysicalDevice device);
		[[nodiscard]] const bool valid_device(VkPhysicalDevice device, std::span<const char*> req_extensions);
		[[nodiscard]] SwapChainInfo query_swap_chain(VkPhysicalDevice device);
		[[nodiscard]] VkSurfaceFormatKHR choose_swap_format(std::span<VkSurfaceFormatKHR> avaliable);
		[[nodiscard]] VkPresentModeKHR choose_swap_mode(std::span<VkPresentModeKHR> avaliable);
		[[nodiscard]] VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		bool m_debug_mode;
		GLFWwindow* m_window;
		VkInstance m_vk_instance;
		VkDebugUtilsMessengerEXT m_debug_messenger;
		VkPhysicalDevice m_gpu;
		VkDevice m_gpu_interface;
		VkQueue m_graphics_queue;
		VkSurfaceKHR m_surface;
		VkQueue m_surface_queue;
		VkSwapchainKHR m_swap_chain;
		VkSurfaceFormatKHR m_swap_format;
		VkPresentModeKHR m_swap_mode;

		std::vector<VkImage> m_swapchain_images;
	};
} // namespace vulkano

#endif