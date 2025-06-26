#pragma once

#include "Core.h"

namespace Bloodshot
{
	struct FSourceMeshDescription final
	{
		uint32_t StartVertex;
		uint32_t StartIndex;
		uint32_t IndexCount;
		uint32_t MaterialIndex;
	};
}