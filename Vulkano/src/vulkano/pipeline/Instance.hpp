#ifndef VULKANO_PIPELINE_INSTANCE_HPP_
#define VULKANO_PIPELINE_INSTANCE_HPP_

#include <optional>
#include <span>

#include <GLFW/glfw3.h>

namespace vulkano
{
	///
	/// Useful to store queue familys that physical device supports when determining valid gpu.
	///
	struct QueueFamilyIndexs
	{
		std::optional<std::uint32_t> m_graphics           = std::nullopt;
		std::optional<std::uint32_t> m_present_to_surface = std::nullopt;

		///
		/// Checks all queue familys and makes sure they are set.
		///
		[[nodiscard]] const bool has_all_required();
	};

	///
	/// Vulkan instance all wrapped up in a class.
	///
	class Instance final
	{
	public:
		struct Settings final
		{
			GLFWwindow* m_window;
			VkApplicationInfo m_settings;
			bool m_debug_mode;
			std::vector<const char*>* m_extensions;
		};

		Instance(const Instance::Settings& settings);
		~Instance();

		[[nodiscard]] VkInstance vk_handle() const;
		[[nodiscard]] VkSurfaceKHR surface() const;
		[[nodiscard]] VkPhysicalDevice physical_device() const;
		[[nodiscard]] VkDevice logical_device() const;
		[[nodiscard]] const QueueFamilyIndexs& qfi() const;

	private:
		[[nodiscard]] QueueFamilyIndexs get_family_indexs(VkPhysicalDevice device);
		[[nodiscard]] const bool valid_device(VkPhysicalDevice device, std::span<const char*> req_extensions);

		bool m_debug_mode;

		VkInstance m_vk_instance;
		VkDebugUtilsMessengerEXT m_debug_messenger;
		VkPhysicalDevice m_gpu;
		VkDevice m_gpu_interface;
		VkQueue m_graphics_queue;
		VkSurfaceKHR m_surface;
		VkQueue m_surface_queue;

		QueueFamilyIndexs m_qfi;
	};
} // namespace vulkano

#endif