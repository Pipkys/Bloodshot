#pragma once

#include "Core.h"

namespace Bloodshot::HeaderTool
{
	bool IsIdentifier(FStringView Word);
	bool IsKeyword(FStringView Word);
	bool IsOperator(const char Char);
	bool IsSeparator(const char Char);
}
