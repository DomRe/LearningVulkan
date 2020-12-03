#ifndef VULKANO_UTILS_META_HPP_
#define VULKANO_UTILS_META_HPP_

#include <type_traits>

namespace vulkano
{
	namespace meta
	{
		///
		/// Useful for specifying a true boolean at runtime.
		///
		struct BoolTrue
		{
			static inline constexpr auto value()
			{
				return true;
			}
		};

		///
		/// Useful for specifying a false boolean at runtime.
		///
		struct BoolFalse
		{
			static inline constexpr auto value()
			{
				return false;
			}
		};

		///
		/// Concept to restrict a type to boolean.
		///
		template<typename Type>
		concept is_constexpr_bool = std::is_same<Type, BoolTrue>::value || std::is_same<Type, BoolFalse>::value;
	} // namespace meta
} // namespace vulkano

#endif