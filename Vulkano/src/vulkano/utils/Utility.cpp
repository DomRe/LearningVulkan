#include "Utility.hpp"

namespace vulkano
{
	const bool QueueFamilyIndexs::has_all_required()
	{
		return m_graphics != std::nullopt;
	}
} // namespace vulkano