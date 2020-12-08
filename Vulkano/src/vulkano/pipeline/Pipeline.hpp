#ifndef VULKANO_GRAPHICS_PIPELINE_HPP_
#define VULKANO_GRAPHICS_PIPELINE_HPP_

#include "vulkano/pipeline/SwapChain.hpp"

namespace vulkano
{
	class Pipeline
	{
	public:
		struct Settings
		{
			VkPolygonMode m_polygon_mode;
			VkCullModeFlags m_cull_mode;
			VkFrontFace m_front_facing;
			VkBool32 m_enable_msaa;
			VkSampleCountFlagBits m_msaa_level;
		};

		struct UpdatedSettings
		{
			glm::vec2 m_viewport_size;
			float m_line_width;
		};

		Pipeline(std::shared_ptr<SwapChain> swapchain, const Pipeline::Settings& settings);
		~Pipeline();

		void reconfigure(const Pipeline::UpdatedSettings& new_settings);

	private:
		std::shared_ptr<Instance> m_instance;

		VkViewport m_viewport;
		VkRect2D m_viewport_scissor;
		VkRenderPass m_render_pass;
		VkPipelineLayout m_layout;
	};
} // namespace vulkano

#endif