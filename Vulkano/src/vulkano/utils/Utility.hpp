#ifndef VULKANO_UTILITY_HPP_
#define VULKANO_UTILITY_HPP_

#include <optional>

namespace vulkano
{
	///
	/// Useful to store queue familys that physical device supports when determining valid gpu.
	///
	struct QueueFamilyIndexs
	{
		std::optional<std::uint32_t> m_graphics           = std::nullopt;
		std::optional<std::uint32_t> m_present_to_surface = std::nullopt;

		///
		/// Checks all queue familys and makes sure they are set.
		///
		[[nodiscard]] const bool has_all_required();
	};

} // namespace vulkano

#endif