#pragma once

#include "Containers/String.h"
#include "LowLevelFormat.h"
#include "Templates/IsCharRange.h"
#include "Templates/Template.h"

namespace Bloodshot::String
{
	template<typename... ArgTypes>
	NODISCARD FString Format(const char* const InFormat, ArgTypes&&... Args)
	{
		Private::String::FLowLevelString Formatted = Private::String::LLFormatImpl(InFormat, std::forward<ArgTypes>(Args)...);
		--Formatted.Size;
		return Formatted;
	}

	template<IsCharRange CharRangeType, typename... ArgTypes>
	NODISCARD FString Format(CharRangeType&& InFormat, ArgTypes&&... Args)
	{
		Private::String::FLowLevelString Formatted = Private::String::LLFormatImpl(GetData(std::forward<CharRangeType>(InFormat)), std::forward<ArgTypes>(Args)...);
		--Formatted.Size;
		return Formatted;
	}

	template<typename... ArgTypes>
	size_t Format(const char* const InFormat, char* const OutBuffer, ArgTypes&&... Args)
	{
		return Private::String::LLFormatImpl(InFormat, OutBuffer, std::forward<ArgTypes>(Args)...);
	}
}
