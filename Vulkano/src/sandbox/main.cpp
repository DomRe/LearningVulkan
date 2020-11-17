#include <iostream>
#include <stdexcept>

#include "vulkano/core/Window.hpp"

namespace vk = vulkano;

class Sandbox
{
public:
	Sandbox(const vk::Window::WindowSettings& window_settings, const VkApplicationInfo& vulkan_settings)
	    : m_window(window_settings, vulkan_settings)
	{
	}

	~Sandbox()
	{
	}

	int run()
	{
		return EXIT_SUCCESS;
	}

private:
	Sandbox() = delete;

private:
	vk::Window m_window;
};

int main()
{
	// clang-format off
	int result = 0;

	try
	{
		Sandbox sandbox
		({
			.width  = 1280,
		    .height = 720,
			.enable_debug = true,
		    .title  = "Sandbox"
		},
		{
			.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		    .pApplicationName   = "Sandbox",
		    .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
		    .pEngineName        = "No Engine",
		    .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
		    .apiVersion         = VK_API_VERSION_1_2
		});
		
		result = sandbox.run();
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		result = EXIT_FAILURE;
	}

	return result;
}