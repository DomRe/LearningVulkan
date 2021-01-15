#ifndef VULKANO_GRAPHICS_IMAGE_HPP_
#define VULKANO_GRAPHICS_IMAGE_HPP_

#include <memory>

#include <vulkan/vulkan.h>

namespace vulkano
{
	struct ImageInfo final
	{
		VkFormat m_format;
		VkImageViewType m_type;
	};

	class Image final
	{
	public:
		Image(std::shared_ptr<Instance> instance, const ImageInfo& info);
		Image(std::shared_ptr<Instance> instance, const ImageInfo& info, VkImage existing);
		~Image();

		[[nodiscard]] VkImage vk_handle() const;
		[[nodiscard]] VkImageView vk_view() const;

	private:
		std::shared_ptr<Instance> m_instance;
		VkImage m_image;
		VkImageView m_view;
	};
} // namespace vulkano

#endif