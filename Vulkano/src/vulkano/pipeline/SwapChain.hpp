#ifndef VULKANO_PIPELINE_SWAPCHAIN_HPP_
#define VULKANO_PIPELINE_SWAPCHAIN_HPP_

#include <span>
#include <vector>

#include <vulkan/vulkan.h>

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
		SwapChain(Instance* instance);
		~SwapChain();

		void recreate();

	private:
		[[nodiscard]] SwapChainInfo query_swap_chain(Instance* instance);
		[[nodiscard]] VkSurfaceFormatKHR choose_swap_format(std::span<VkSurfaceFormatKHR> avaliable);
		[[nodiscard]] VkPresentModeKHR choose_swap_mode(std::span<VkPresentModeKHR> avaliable);
		[[nodiscard]] VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkSwapchainKHR m_swap_chain;
		VkSurfaceFormatKHR m_format;
		VkPresentModeKHR m_mode;
		VkFormat m_image_format;
		VkExtent2D m_extent;

		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_image_views;
	};
} // namespace vulkano

#endif