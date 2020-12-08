#include "vulkano/pipeline/Instance.hpp"
#include "vulkano/utils/Log.hpp"

#include "Image.hpp"

namespace vulkano
{
	Image::Image(std::shared_ptr<Instance> instance, const ImageInfo& info)
	    : m_instance {instance}, m_image {nullptr}, m_view {nullptr}
	{
	}

	Image::Image(std::shared_ptr<Instance> instance, const ImageInfo& info, VkImage existing)
	    : m_instance {instance}, m_image {existing}, m_view {nullptr}
	{
		// clang-format off
		VkImageViewCreateInfo image_view_info
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.image = m_image,
			.viewType = info.m_type,
			.format = info.m_format
		};
		// clang-format on

		image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		image_view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_info.subresourceRange.baseMipLevel   = 0;
		image_view_info.subresourceRange.levelCount     = 1;
		image_view_info.subresourceRange.baseArrayLayer = 0;
		image_view_info.subresourceRange.layerCount     = 1;

		if (vkCreateImageView(m_instance->logical_device(), &image_view_info, nullptr, &m_view) != VK_SUCCESS)
		{
			VK_LOG(VK_THROW, "Failed to create image view.");
		}
	}

	Image::~Image()
	{
		vkDestroyImageView(m_instance->logical_device(), m_view, nullptr);
	}

	VkImage Image::vk_handle() const
	{
		return m_image;
	}

	VkImageView Image::vk_view() const
	{
		return m_view;
	}
} // namespace vulkano