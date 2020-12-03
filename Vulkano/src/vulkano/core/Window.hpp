#ifndef VULKANO_CORE_WINDOW_HPP_
#define VULKANO_CORE_WINDOW_HPP_

#include <memory>
#include <vector>

#include "vulkano/pipeline/Instance.hpp"
#include "vulkano/pipeline/SwapChain.hpp"

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
		Window() = delete;

	private:
		GLFWwindow* m_window;

		std::unique_ptr<Instance> m_instance;
		std::unique_ptr<SwapChain> m_swapchain;
	};
} // namespace vulkano

#endif