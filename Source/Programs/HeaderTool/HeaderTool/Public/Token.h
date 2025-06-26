#pragma once

#include "Core.h"

#include <cstdint>

namespace Bloodshot::HeaderTool
{
	enum class ETokenType : uint8_t
	{
		Keyword = 0,
		Identifier,
		NumberLiteral,
		StringLiteral,
		Operator,
		ScopeResolutionOperator,
		Separator,
		Comment,
		Unknown
	};

	struct FToken final
	{
		ETokenType Type = ETokenType::Unknown;
		FString Value;
	};
}
