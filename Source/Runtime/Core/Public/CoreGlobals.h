#pragma once

#include "Platform/Platform.h"
#include "Containers/StringView.h"
#include "String/Format.h"

namespace Bloodshot
{
	constexpr inline const char* ResourcePath = "../Resources/";

	NODISCARD FORCEINLINE FString BuildResourcePath(FStringView FilePath)
	{
		return String::Format("{}{}", ResourcePath, FilePath);
	}
}
