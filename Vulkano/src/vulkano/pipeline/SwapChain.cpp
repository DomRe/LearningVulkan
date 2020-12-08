#include <array>

#include "vulkano/pipeline/Instance.hpp"
#include "vulkano/utils/Log.hpp"

#include "SwapChain.hpp"

namespace vulkano
{
	const bool SwapChainInfo::is_valid()
	{
		return (!m_formats.empty()) && (!m_present_modes.empty());
	}

	SwapChain::SwapChain(std::shared_ptr<Instance> instance, const glm::vec2& framebuffer_size)
	    : m_instance {instance}, m_swap_chain {nullptr}, m_framebuffer_size {framebuffer_size}
	{
		auto swap_chain_info = query_swap_chain();
		if (!swap_chain_info.is_valid())
		{
			VK_LOG(VK_THROW, "Provided physical device does not support swapchain.");
		}
		else
		{
			m_format       = choose_swap_format(swap_chain_info.m_formats);
			m_mode         = choose_swap_mode(swap_chain_info.m_present_modes);
			m_extent       = choose_swap_extent(swap_chain_info.m_capabilities);
			m_image_format = m_format.format;

			std::uint32_t image_count = swap_chain_info.m_capabilities.minImageCount + 1;
			if ((swap_chain_info.m_capabilities.maxImageCount > 0) && (image_count > swap_chain_info.m_capabilities.maxImageCount))
			{
				image_count = swap_chain_info.m_capabilities.maxImageCount;
			}

			// clang-format off
			VkSwapchainCreateInfoKHR create_swapchain_info
			{
			    .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			    .pNext            = nullptr,
			    .flags            = 0,
			    .surface          = m_instance->surface(),
			    .minImageCount    = image_count,
			    .imageFormat      = m_image_format,
			    .imageColorSpace  = m_format.colorSpace,
			    .imageExtent      = m_extent,
			    .imageArrayLayers = 1,
			    .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			    .preTransform     = swap_chain_info.m_capabilities.currentTransform,
			    .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			    .presentMode      = m_mode,
			    .clipped          = VK_TRUE,
			    .oldSwapchain     = VK_NULL_HANDLE};

			std::array<std::uint32_t, 2> qfi_indexs = {m_instance->qfi().m_graphics.value(), m_instance->qfi().m_present_to_surface.value()};
			if (qfi_indexs[0] != qfi_indexs[1])
			{
				create_swapchain_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
				create_swapchain_info.queueFamilyIndexCount = static_cast<std::uint32_t>(qfi_indexs.size());
				create_swapchain_info.pQueueFamilyIndices   = qfi_indexs.data();
			}
			else
			{
				create_swapchain_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
				create_swapchain_info.queueFamilyIndexCount = 0;
				create_swapchain_info.pQueueFamilyIndices   = nullptr;
			}

			if (vkCreateSwapchainKHR(m_instance->logical_device(), &create_swapchain_info, nullptr, &m_swap_chain) != VK_SUCCESS)
			{
				VK_LOG(VK_THROW, "Failed to create window swap chain.");
			}
			else
			{
				vkGetSwapchainImagesKHR(m_instance->logical_device(), m_swap_chain, &image_count, nullptr);

				std::vector<VkImage> swap_imgs {image_count};
				m_images.resize(image_count);

				vkGetSwapchainImagesKHR(m_instance->logical_device(), m_swap_chain, &image_count, swap_imgs.data());

				for (std::size_t i = 0; i < image_count; i++)
				{
					ImageInfo info
					{
						.m_format = m_image_format,
						.m_type = VK_IMAGE_VIEW_TYPE_2D
					};
					m_images[i] = std::make_unique<Image>(info, swap_imgs[i]);
				}
			}
		}
	}

	SwapChain::~SwapChain()
	{
		m_images.clear();
		vkDestroySwapchainKHR(m_instance->logical_device(), m_swap_chain, nullptr);
	}

	void SwapChain::recreate()
	{
	}

	const VkExtent2D* SwapChain::extent()
	{
		return &m_extent;
	}

	std::shared_ptr<Instance> SwapChain::instance_used()
	{
		return m_instance;
	}

	const VkFormat SwapChain::image_format() const
	{
		return m_image_format;
	}

	SwapChainInfo SwapChain::query_swap_chain()
	{
		SwapChainInfo info;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_instance->physical_device(), m_instance->surface(), &info.m_capabilities);

		std::uint32_t surface_format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_instance->physical_device(), m_instance->surface(), &surface_format_count, nullptr);

		if (surface_format_count != 0)
		{
			info.m_formats.resize(surface_format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_instance->physical_device(), m_instance->surface(), &surface_format_count, info.m_formats.data());
		}

		std::uint32_t presentation_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_instance->physical_device(), m_instance->surface(), &presentation_mode_count, nullptr);

		if (presentation_mode_count != 0)
		{
			info.m_present_modes.resize(presentation_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_instance->physical_device(), m_instance->surface(), &presentation_mode_count, info.m_present_modes.data());
		}

		return std::move(info);
	}

	VkSurfaceFormatKHR SwapChain::choose_swap_format(std::span<VkSurfaceFormatKHR> avaliable)
	{
		for (const auto& format : avaliable)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		// Backup choice.
		return avaliable[0];
	}

	VkPresentModeKHR SwapChain::choose_swap_mode(std::span<VkPresentModeKHR> avaliable)
	{
		for (const auto& mode : avaliable)
		{
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return mode;
			}
		}

		// Backup choice.
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D extent =
			    {
				.width  = std::clamp(static_cast<std::uint32_t>(m_framebuffer_size.x), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
				.height = std::clamp(static_cast<std::uint32_t>(m_framebuffer_size.y), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};

			return extent;
		}
	}
} // namespace vulkano