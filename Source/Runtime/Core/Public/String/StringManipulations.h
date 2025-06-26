#pragma once

#include "Containers/StringFwd.h"
#include "Templates/IsCharRange.h"
#include "Templates/IsCharType.h"
#include "Templates/Template.h"

#include <functional>

namespace Bloodshot::String
{
	void ParseTokens(FStringView InView, FStringView InDelimiter, const std::function<void(FStringView)>& InVisitor);
	void Replace(FString& InString, FStringView InSubStrToReplace, FStringView InNewSubStr);

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr size_t GetSize(const T Char)
	{
		return 1;
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr size_t GetSize(const T* String)
	{
		return std::char_traits<char>::length(String);
	}

	template<IsCharRange T>
	NODISCARD FORCEINLINE constexpr size_t GetSize(T&& Range)
	{
		return std::char_traits<char>::length(GetData(std::forward<T>(Range)));
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr bool IsEqual(const T FirstChar, const T SecondChar)
	{
		return FirstChar == SecondChar;
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr bool IsEqual(const T Char, const T* String)
	{
		const size_t StringSize = std::char_traits<char>::length(String);
		return StringSize == 1 && !std::char_traits<char>::compare(&Char, String, 1);
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr bool IsEqual(const T* String, const T Char)
	{
		const size_t StringSize = std::char_traits<char>::length(String);
		return StringSize == 1 && !std::char_traits<char>::compare(&Char, String, 1);
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr bool IsEqual(const T* FirstString, const T* SecondString)
	{
		const size_t FirstSize = std::char_traits<char>::length(FirstString);
		return FirstSize == std::char_traits<char>::length(SecondString)
			&& !std::char_traits<char>::compare(FirstString, SecondString, FirstSize);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr bool IsEqual(const CharType Char, RangeType&& Range)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		const size_t RangeSize = std::char_traits<char>::length(RangeData);
		return RangeSize == 1 && RangeData[0] == Char;
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr bool IsEqual(RangeType&& Range, const CharType Char)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		const size_t RangeSize = std::char_traits<char>::length(RangeData);
		return RangeSize == 1 && RangeData[0] == Char;
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr bool IsEqual(const CharType* String, RangeType&& Range)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		const size_t RangeSize = std::char_traits<char>::length(RangeData);
		return RangeSize == std::char_traits<char>::length(String)
			&& !std::char_traits<char>::compare(RangeData, String, RangeSize);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr bool IsEqual(const CharType* String, RangeType&& Range, const size_t Count)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		return !std::char_traits<char>::compare(RangeData, String, Count);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr bool IsEqual(RangeType&& Range, const CharType* String)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		const size_t RangeSize = std::char_traits<char>::length(RangeData);
		return RangeSize == std::char_traits<char>::length(String)
			&& !std::char_traits<char>::compare(RangeData, String, RangeSize);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr bool IsEqual(RangeType&& Range, const CharType* String, const size_t Count)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		return !std::char_traits<char>::compare(RangeData, String, Count);
	}

	template<IsCharRange T>
	NODISCARD FORCEINLINE constexpr bool IsEqual(T&& FirstRange, T&& SecondRange)
	{
		auto FirstData = GetData(std::forward<T>(FirstRange));
		auto SecondData = GetData(std::forward<T>(SecondRange));
		const size_t FirstSize = std::char_traits<char>::length(FirstData);
		const size_t SecondSize = std::char_traits<char>::length(SecondData);
		return FirstSize == SecondSize && !std::char_traits<char>::compare(FirstData, SecondData);
	}

	template<IsCharRange T>
	NODISCARD FORCEINLINE constexpr bool IsEqual(T&& FirstRange, T&& SecondRange, const size_t Count)
	{
		return !std::char_traits<char>::compare(GetData(std::forward<T>(FirstRange)),
			GetData(std::forward<T>(SecondRange)),
			Count);
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr int Compare(const T FirstChar, const T SecondChar)
	{
		return std::char_traits<char>::compare(&FirstChar, &SecondChar, 1);
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr int Compare(const T Char, const T* String)
	{
		const size_t StringSize = std::char_traits<char>::length(String);
		return StringSize != 1
			? -1
			: std::char_traits<char>::compare(&Char, String, 1);
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr int Compare(const T* String, const T Char)
	{
		const size_t StringSize = std::char_traits<char>::length(String);
		return StringSize != 1
			? -1
			: std::char_traits<char>::compare(&Char, String, 1);
	}

	template<IsCharType T>
	NODISCARD FORCEINLINE constexpr int Compare(const T* FirstString, const T* SecondString)
	{
		const size_t FirstSize = std::char_traits<char>::length(FirstString);
		return FirstSize != std::char_traits<char>::length(SecondString)
			? -1
			: std::char_traits<char>::compare(FirstString, SecondString, FirstSize);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr int Compare(const CharType Char, RangeType&& Range)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		const size_t RangeSize = std::char_traits<char>::length(RangeData);
		return RangeSize != 1
			? -1
			: std::char_traits<char>::compare(RangeData, &Char, 1);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr int Compare(RangeType&& Range, const CharType Char)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		const size_t RangeSize = std::char_traits<char>::length(RangeData);
		return RangeSize != 1
			? -1
			: std::char_traits<char>::compare(RangeData, &Char, 1);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr int Compare(const CharType* String, RangeType&& Range)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		const size_t RangeSize = std::char_traits<char>::length(RangeData);
		return RangeSize != std::char_traits<char>::length(String)
			? -1
			: std::char_traits<char>::compare(RangeData, String, RangeSize);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr int Compare(const CharType* String, RangeType&& Range, const size_t Count)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		return std::char_traits<char>::compare(RangeData, String, Count);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr int Compare(RangeType&& Range, const CharType* String)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		const size_t RangeSize = std::char_traits<char>::length(RangeData);
		return RangeSize != std::char_traits<char>::length(String)
			? -1
			: std::char_traits<char>::compare(RangeData, String, RangeSize);
	}

	template<IsCharType CharType, IsCharRange RangeType>
	NODISCARD FORCEINLINE constexpr int Compare(RangeType&& Range, const CharType* String, const size_t Count)
	{
		auto RangeData = GetData(std::forward<RangeType>(Range));
		return std::char_traits<char>::compare(RangeData, String, Count);
	}

	template<IsCharRange T>
	NODISCARD FORCEINLINE constexpr int Compare(T&& FirstRange, T&& SecondRange)
	{
		auto FirstData = GetData(std::forward<T>(FirstRange));
		auto SecondData = GetData(std::forward<T>(SecondRange));
		const size_t FirstSize = std::char_traits<char>::length(FirstData);
		const size_t SecondSize = std::char_traits<char>::length(SecondData);
		return FirstSize != SecondSize
			? -1
			: std::char_traits<char>::compare(FirstData, SecondData, FirstSize);
	}

	template<IsCharRange T>
	NODISCARD FORCEINLINE constexpr int Compare(T&& FirstRange, T&& SecondRange, const size_t Count)
	{
		return std::char_traits<char>::compare(GetData(std::forward<T>(FirstRange)),
			GetData(std::forward<T>(SecondRange)),
			Count);
	}
}
