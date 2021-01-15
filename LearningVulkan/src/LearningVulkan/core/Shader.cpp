#include <filesystem>
#include <fstream>
#include <span>

#include "vulkano/utils/Log.hpp"

#include "Shader.hpp"

namespace vulkano
{
	Shader::Shader(VkDevice logical, std::string_view vertex, std::string_view fragment)
	    : m_logical(logical)
	{
		const auto vert_shader = read(vertex);
		const auto frag_shader = read(fragment);

		VkShaderModule vert_shader_module = create_module(vert_shader);
		VkShaderModule frag_shader_module = create_module(frag_shader);

		// clang-format off
		VkPipelineShaderStageCreateInfo vert_create_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vert_shader_module,
			.pName = "main",
			.pSpecializationInfo = nullptr

		};

		VkPipelineShaderStageCreateInfo frag_create_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = frag_shader_module,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};
		// clang-format on

		VkPipelineShaderStageCreateInfo stages[] = {vert_create_info, frag_create_info};

		vkDestroyShaderModule(m_logical, vert_shader_module, nullptr);
		vkDestroyShaderModule(m_logical, frag_shader_module, nullptr);

		// clang-format off
		VkPipelineVertexInputStateCreateInfo vertex_input_info 
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions = nullptr,
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr
		};
		// clang-format on
	}

	Shader::~Shader()
	{
	}

	std::span<char> Shader::read(std::string_view path)
	{
		auto file = std::filesystem::path {path};
		std::ifstream ifs;

		ifs.open(file.string(), std::ifstream::ate | std::ifstream::binary);
		if (!ifs.is_open())
		{
			VK_LOG(VK_THROW, "Failed to open shader: {0}.", path);
		}
		const std::size_t size = static_cast<std::size_t>(ifs.tellg());

		std::vector<char> buffer;
		buffer.resize(size);
		ifs.seekg(0);
		ifs.read(buffer.data(), size);
		ifs.close();

		return buffer;
	}

	VkShaderModule Shader::create_module(std::span<char> code)
	{
		// clang-format off
		VkShaderModuleCreateInfo create_info
		{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_NULL_HANDLE,
			.codeSize = code.size(),
			.pCode = reinterpret_cast<const std::uint32_t*>(code.data())
		};
		// clang-format on

		VkShaderModule shader_module;
		if (vkCreateShaderModule(m_logical, &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			VK_LOG(VK_THROW, "Failed to create shader module.");
		}

		return shader_module;
	}
} // namespace vulkano