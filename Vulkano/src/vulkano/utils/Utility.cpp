#include "Utility.hpp"

namespace vulkano
{
	const bool QueueFamilyIndexs::has_all_required()
	{
		return (m_graphics != std::nullopt) && (m_present_to_surface != std::nullopt);
	}
} // namespace vulkano