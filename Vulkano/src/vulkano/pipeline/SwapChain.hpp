#ifndef VULKANO_PIPELINE_SWAPCHAIN_HPP_
#define VULKANO_PIPELINE_SWAPCHAIN_HPP_

#include <span>
#include <vector>

#include <glm/vec2.hpp>

#include "vulkano/graphics/Image.hpp"

namespace vulkano
{
	class Instance;

	///
	/// Stores information about SwapChain support that can be easily passed around.
	///
	struct SwapChainInfo
	{
		VkSurfaceCapabilitiesKHR m_capabilities;

		std::vector<VkSurfaceFormatKHR> m_formats;
		std::vector<VkPresentModeKHR> m_present_modes;

		const bool is_valid();
	};

	///
	/// Actual swapchain class.
	///
	class SwapChain final
	{
	public:
		SwapChain(std::shared_ptr<Instance> instance, const glm::vec2& framebuffer_size);
		~SwapChain();

		void recreate();

		[[nodiscard]] const VkExtent2D* extent();
		[[nodiscard]] std::shared_ptr<Instance> instance_used();
		[[nodiscard]] const VkFormat image_format() const;

	private:
		[[nodiscard]] SwapChainInfo query_swap_chain();
		[[nodiscard]] VkSurfaceFormatKHR choose_swap_format(std::span<VkSurfaceFormatKHR> avaliable);
		[[nodiscard]] VkPresentModeKHR choose_swap_mode(std::span<VkPresentModeKHR> avaliable);
		[[nodiscard]] VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

		std::shared_ptr<Instance> m_instance;
		VkSwapchainKHR m_swap_chain;
		VkSurfaceFormatKHR m_format;
		VkPresentModeKHR m_mode;
		VkFormat m_image_format;
		VkExtent2D m_extent;

		glm::vec2 m_framebuffer_size;

		std::vector<std::unique_ptr<Image>> m_images;
	};
} // namespace vulkano

#endif