#pragma once

#include "Containers/StringFwd.h"
#include "Misc/CoreMisc.h"
#include "Platform/Platform.h"
#include "String/StringManipulations.h"
#include "Templates/IsCharRange.h"
#include "Templates/Template.h"
#include "Templates/TypeHash.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	template<typename InCharType>
	class TStringView
	{
	public:
		using ElementType = InCharType;

		FORCEINLINE constexpr TStringView() = default;

		FORCEINLINE constexpr TStringView(const TStringView& Other)
			: Data(Other.Data)
			, Size(Other.Size)
		{
		}

		FORCEINLINE constexpr TStringView(TStringView&& Other) noexcept
			: Data(Other.Data)
			, Size(Other.Size)
		{
			Other.Data = nullptr;
			Other.Size = 0;
		}

		FORCEINLINE constexpr TStringView(const ElementType* InStr)
			: Data(InStr)
			, Size(String::GetSize(InStr))
		{
		}

		FORCEINLINE constexpr TStringView(const ElementType* InStr, const size_t InSize)
			: Data(InStr)
			, Size(InSize)
		{
		}

		template<IsCharRange T>
		FORCEINLINE constexpr TStringView(T&& Range)
			: TStringView(Bloodshot::GetData(Range), Bloodshot::GetSize(std::forward<T>(Range)))
		{
		}

		FORCEINLINE constexpr ~TStringView() = default;

		NODISCARD FORCEINLINE constexpr const ElementType& operator[](const size_t Index) const
		{
			return Data[Index];
		}

		FORCEINLINE constexpr TStringView& operator=(const TStringView& Other)
		{
			Data = Other.Data;
			Size = Other.Size;
			return *this;
		}

		FORCEINLINE constexpr TStringView& operator=(TStringView&& Other) noexcept
		{
			Data = Other.Data;
			Size = Other.Size;
			Other.Data = nullptr;
			Other.Size = 0;
			return *this;
		}

		FORCEINLINE constexpr bool operator==(const TStringView& Other) const
		{
			return String::IsEqual(Data, Other.Data);
		}

		FORCEINLINE constexpr bool operator==(const ElementType* String) const
		{
			return String::IsEqual(Data, String);
		}

		NODISCARD FORCEINLINE constexpr const ElementType* GetData() const
		{
			return Data;
		}

		NODISCARD FORCEINLINE constexpr size_t GetSize() const
		{
			return Size;
		}

		NODISCARD FORCEINLINE constexpr bool IsEmpty() const
		{
			return !Size;
		}

		NODISCARD FORCEINLINE constexpr TStringView SubStr(const size_t Index, const size_t Count) const
		{
			return TStringView(Data + Index, Count);
		}

		NODISCARD FORCEINLINE constexpr TStringView SubStr(const size_t Index) const
		{
			return TStringView(Data + Index, Size - Index);
		}

		NODISCARD FORCEINLINE constexpr size_t Find(const ElementType* const SubStr) const
		{
			const size_t SubStrSize = String::GetSize(SubStr);
			size_t SameCharCount = 0;
			for (size_t i = 0; i < SubStrSize; ++i)
			{
				if (Data[i] == SubStr[i])
				{
					++SameCharCount;
				}

				if (SameCharCount == SubStrSize)
				{
					return i - SubStrSize + 1;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE constexpr bool Find(const ElementType* const SubStr, size_t& OutIndex) const
		{
			OutIndex = Find(SubStr);
			return OutIndex != IReservedValues::NoneIndex;
		}

		template<typename T>
		NODISCARD FORCEINLINE constexpr size_t Find(T&& Range) const
		{
			return Find(Bloodshot::GetData(std::forward<T>(Range)));
		}

		NODISCARD FORCEINLINE constexpr size_t Find(TStringView Other) const
		{
			return Find(Other.Data);
		}

		NODISCARD FORCEINLINE constexpr bool Find(TStringView Other, size_t& OutIndex) const
		{
			return Find(Other.Data, OutIndex);
		}

		NODISCARD FORCEINLINE constexpr size_t FindChar(const ElementType Char) const
		{
			for (size_t i = 0; i < Size; ++i)
			{
				if (Data[i] == Char)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE constexpr bool FindChar(const ElementType Char, size_t& OutIndex) const
		{
			OutIndex = FindChar(Char);
			return OutIndex != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE constexpr size_t FindLastChar(const ElementType Char) const
		{
			for (size_t i = Size; i > 0; --i)
			{
				if (Data[i - 1] == Char)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE constexpr bool FindLastChar(const ElementType Char, size_t& OutIndex) const
		{
			OutIndex = FindLastChar(Char);
			return OutIndex != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE constexpr bool Contains(const ElementType* const SubStr) const
		{
			return Find(SubStr) != IReservedValues::NoneIndex;
		}

		template<typename T>
		NODISCARD FORCEINLINE constexpr bool Contains(T&& Range) const
		{
			return Find(std::forward<T>(Range)) != IReservedValues::NoneIndex;
		}

		// For internal usage only!
		FORCEINLINE constexpr const ElementType* begin() const { return Data; }
		FORCEINLINE constexpr const ElementType* end() const { return Data + Size; }

	private:
		const ElementType* Data = nullptr;
		size_t Size = 0;
	};

	template<typename CharType>
	struct TIsZeroConstructible<TStringView<CharType>>
	{
		static constexpr bool Value = true;
	};

	NODISCARD FORCEINLINE uint64_t GetTypeHash(FStringView Str)
	{
		return Private::TypeHash::GetFnv1aHashOfArrayRepresentation(Str.GetData(), Str.GetSize());
	}
}
