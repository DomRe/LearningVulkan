#include <array>

#include "vulkano/pipeline/Instance.hpp"
#include "vulkano/utils/Log.hpp"

#include "Pipeline.hpp"

namespace vulkano
{
	Pipeline::Pipeline(std::shared_ptr<SwapChain> swapchain, const Pipeline::Settings& settings)
	    : m_instance {nullptr}, m_layout {nullptr}
	{
		const auto* swap_extent = swapchain->extent();
		m_instance              = swapchain->instance_used();

		// clang-format off
		VkAttachmentDescription colour_attachment
		{
			.flags = VK_NULL_HANDLE,
			.format = swapchain->image_format(),
			.samples = settings.m_msaa_level,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colour_attachment_ref
		{
			.attachment = 0,
		    .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};
		
		VkSubpassDescription subpass_desc
		{
			.flags = VK_NULL_HANDLE,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colour_attachment_ref,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = nullptr,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
		};

		VkRenderPassCreateInfo render_pass_info
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.attachmentCount = 1,
			.pAttachments = &colour_attachment,
			.subpassCount = 1,
			.pSubpasses = &subpass_desc,
			.dependencyCount = 0,
			.pDependencies = nullptr
		};

		if (vkCreateRenderPass(m_instance->logical_device(), &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
		{
			VK_LOG(VK_THROW, "Failed to create render pass.");
		}

		VkPipelineInputAssemblyStateCreateInfo input_assembly
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		
		// This is where on the swapchain images the output should be drawn.
		// See:
		// https://vulkan-tutorial.com/images/viewports_scissors.png
		
		m_viewport =
		{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(swap_extent->width),
			.height = static_cast<float>(swap_extent->height),
			.minDepth = 0.0f,
		};

		// This is the area of the pixels that should be drawn to the viewport.
		m_viewport_scissor =
		{
			.offset =
			{
				.x = 0,
				.y = 0
			},
			.extent = *swap_extent
		};

		VkPipelineViewportStateCreateInfo viewport_state_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.viewportCount = 1,
			.pViewports = &m_viewport,
			.scissorCount = 1,
			.pScissors = &m_viewport_scissor
		};

		VkPipelineRasterizationStateCreateInfo rasterizer_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = settings.m_polygon_mode,
			.cullMode = settings.m_cull_mode,
            .frontFace = settings.m_front_facing,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampling_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.rasterizationSamples = settings.m_msaa_level,
			.sampleShadingEnable = settings.m_enable_msaa,
			.minSampleShading = 1.0f,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		VkPipelineColorBlendAttachmentState blending_attachment
		{
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo blending_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &blending_attachment
		};
		
		blending_info.blendConstants[0] = 0.0f;
		blending_info.blendConstants[1] = 0.0f;
		blending_info.blendConstants[2] = 0.0f;
		blending_info.blendConstants[3] = 0.0f;

		/*
		const constexpr std::array<VkDynamicState, 2> dynamic_states =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamic_states_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.dynamicStateCount = 2,
			.pDynamicStates = dynamic_states.data()
		};
		*/

		VkPipelineLayoutCreateInfo layout_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr,
		};
		// clang-format on

		if (vkCreatePipelineLayout(m_instance->logical_device(), &layout_info, nullptr, &m_layout) != VK_SUCCESS)
		{
			VK_LOG(VK_THROW, "Failed to create pipeline layout.");
		}
	}

	Pipeline::~Pipeline()
	{
		vkDestroyPipelineLayout(m_instance->logical_device(), m_layout, nullptr);
		vkDestroyRenderPass(m_instance->logical_device(), m_render_pass, nullptr);
	}

	void Pipeline::reconfigure(const Pipeline::UpdatedSettings& new_settings)
	{
	}
} // namespace vulkano