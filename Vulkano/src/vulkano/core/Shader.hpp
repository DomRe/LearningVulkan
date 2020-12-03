#ifndef VULKANO_CORE_SHADER_HPP_
#define VULKANO_CORE_SHADER_HPP_

#include <vulkan/vulkan.h>

#include <string_view>

namespace vulkano
{
	class Shader
	{
	public:
		Shader(VkDevice logical, std::string_view vertex, std::string_view fragment);
		~Shader();

		//void define_specialization();

	private:
		std::span<char> read(std::string_view path);
		VkShaderModule create_module(std::span<char> code);

		VkDevice m_logical;
	};
} // namespace vulkano

#endif