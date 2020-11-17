#ifndef VULKANO_LOG_HPP_
#define VULKANO_LOG_HPP_

#include <iostream>
#include <string_view>

#include <fmt/format.h>

#include "vulkano/utils/Meta.hpp"

// clang-format off
#define VK_THROW    vulkano::meta::BoolTrue{}
#define VK_NO_THROW vulkano::meta::BoolFalse{}
#define VK_LOG(...) vulkano::Log::get().log(__VA_ARGS__)
// clang-format on

namespace vulkano
{
	class Log final
	{
	public:
		~Log() = default;

		static Log& get();

		template<meta::is_constexpr_bool throw_except_type, typename... MsgInputs>
		void log(const throw_except_type, std::string_view message, const MsgInputs&... args /*, std::source_location location = */);

	private:
		Log() = default;

		void set_colour();
	};

	template<meta::is_constexpr_bool throw_except_type, typename... MsgInputs>
	inline void Log::log(const throw_except_type, std::string_view message, const MsgInputs&... args)
	{
		set_colour();
		auto formatted = fmt::format(message, args...);

		std::cout << formatted << std::endl;
		if constexpr (throw_except_type::value())
		{
			throw std::runtime_error(formatted);
		}
	}
} // namespace vulkano

#endif