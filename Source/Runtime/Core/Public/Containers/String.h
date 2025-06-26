#pragma once

#include "Allocators/Allocator.h"
#include "Containers/Array.h"
#include "Containers/StringFwd.h"
#include "Containers/StringView.h"
#include "Misc/CoreMisc.h"
#include "String/StringManipulations.h"
#include "Templates/IsCharRange.h"
#include "Templates/MemoryOperations.h"
#include "Templates/Template.h"
#include "Templates/TypeHash.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	template<typename InCharType, IsAllocator InAllocatorType>
	class TString
	{
	public:
		using ElementType = InCharType;
		using AllocatorType = InAllocatorType::template ForElementType<ElementType>;

		static_assert(IsContiguousAllocator<AllocatorType>, "TString requires contiguous memory allocator");

	private:
		using DataType = TArray<ElementType, InAllocatorType>;
		DataType Data;

	public:
		FORCEINLINE TString() = default;
		FORCEINLINE TString(const TString& Other) = default;
		FORCEINLINE TString(TString&& Other) noexcept = default;
		FORCEINLINE TString& operator=(const TString& Other) = default;
		FORCEINLINE TString& operator=(TString&& Other) noexcept = default;
		FORCEINLINE TString(const AllocatorType& Allocator) : Data(Allocator) {}
		FORCEINLINE TString(std::initializer_list<ElementType> InitList) : Data(InitList) {}

		FORCEINLINE TString(const ElementType Char)
		{
			AppendChar(Char);
		}

		FORCEINLINE TString(const ElementType* String)
		{
			AppendChars(String);
		}

		FORCEINLINE TString(const ElementType* String, const size_t Size)
		{
			AppendChars(String, Size);
		}

		template<IsCharRange T>
		FORCEINLINE TString(T&& Range)
		{
			const ElementType* RangeData = Bloodshot::GetData(Range);
			const size_t RangeSize = Bloodshot::GetSize(std::forward<T>(Range));
			AppendChars(RangeData, RangeSize);
		}

		FORCEINLINE ~TString()
		{
			Dispose();
		}

		FORCEINLINE TString& operator=(const ElementType Char)
		{
			Clear();
			return AppendChar(Char);
		}

		FORCEINLINE TString& operator=(const ElementType* String)
		{
			Clear();
			return AppendChars(String);
		}

		template<IsCharRange T>
		FORCEINLINE TString& operator=(T&& Range)
		{
			Clear();
			return Append(std::forward<T>(Range));
		}

		FORCEINLINE TString& operator+=(const ElementType Char)
		{
			return AppendChar(Char);
		}

		FORCEINLINE TString& operator+=(const ElementType* String)
		{
			return Append(String);
		}

		template<IsCharRange T>
		FORCEINLINE TString& operator+=(T&& Range)
		{
			return Append(std::forward<T>(Range));
		}

		FORCEINLINE TString operator+(const ElementType Char) const
		{
			FString NewString = *this;
			NewString.AppendChar(Char);
			return NewString;
		}

		FORCEINLINE TString operator+(const ElementType* String) const
		{
			FString NewString = *this;
			NewString.Append(String);
			return NewString;
		}

		template<IsCharRange T>
		FORCEINLINE TString operator+(T&& Range) const
		{
			FString NewString = *this;
			NewString.Append(std::forward<T>(Range));
			return NewString;
		}

		NODISCARD FORCEINLINE ElementType& operator[](const size_t Index)
		{
			return Data[Index];
		}

		NODISCARD FORCEINLINE const ElementType& operator[](const size_t Index) const
		{
			return Data[Index];
		}

		NODISCARD FORCEINLINE bool operator==(const FString& Other) const
		{
			return String::IsEqual(GetData(), Other);
		}

		NODISCARD FORCEINLINE bool operator==(const ElementType Char) const
		{
			return String::IsEqual(GetData(), Char);
		}

		NODISCARD FORCEINLINE bool operator==(const ElementType* String) const
		{
			return String::IsEqual(GetData(), String);
		}

		template<IsCharRange T>
		NODISCARD FORCEINLINE bool operator==(T&& Range) const
		{
			return String::IsEqual(GetData(), std::forward<T>(Range));
		}

		NODISCARD FORCEINLINE AllocatorType& GetAllocator()
		{
			return Data.GetAllocator();
		}

		NODISCARD FORCEINLINE const AllocatorType& GetAllocator() const
		{
			return Data.GetAllocator();
		}

		NODISCARD FORCEINLINE DataType& GetCharArray()
		{
			return Data;
		}

		NODISCARD FORCEINLINE const DataType& GetCharArray() const
		{
			return Data;
		}

		NODISCARD FORCEINLINE ElementType* GetData()
		{
			return Data.GetData();
		}

		NODISCARD FORCEINLINE const ElementType* GetData() const
		{
			return Data.GetData();
		}

		NODISCARD FORCEINLINE size_t GetSize() const
		{
			const size_t Size = Data.GetSize();
			return Size ? Size - 1 : 0;
		}
		
		NODISCARD FORCEINLINE size_t GetCapacity() const
		{
			const size_t Capacity = Data.GetCapacity();
			return Capacity ? Capacity - 1 : 0;
		}

		NODISCARD FORCEINLINE bool IsEmpty() const
		{
			return Data.IsEmpty();
		}

		NODISCARD FORCEINLINE bool IsValidIndex(const size_t Index) const
		{
			return Data.IsValidIndex(Index);
		}

		FORCEINLINE void Init(const size_t Size, const ElementType& Element = ElementType())
		{
			Data.Init(Size, Element);
		}

		FORCEINLINE void Reserve(const size_t NewCapacity)
		{
			Data.Reserve(NewCapacity);
		}

		FORCEINLINE void Shrink()
		{
			Data.Shrink();
		}

		FORCEINLINE void Clear()
		{
			Data.Clear();
		}

		FORCEINLINE void Dispose()
		{
			Data.Dispose();
		}

		TString& AppendChar(const ElementType Char)
		{
			if (Char != 0)
			{
				const size_t OldSize = Data.GetSize();
				const size_t InsertIndex = OldSize ? OldSize - 1 : 0;
				const size_t InsertCount = OldSize ? 1 : 2;
				Data.AddUninitialized(InsertCount);
				Data[InsertIndex] = Char;
				Data[InsertIndex + 1] = '\0';
			}

			return *this;
		}

		TString& AppendChars(const ElementType* const InString, const size_t InSize)
		{
			if (!InSize)
			{
				return *this;
			}

			BS_CHECK(InString);

			const size_t OldSize = Data.GetSize();
			Data.AddUninitialized(InSize + (OldSize ? 0 : 1));

			ElementType* const RawData = GetData();
			const size_t OldEnd = OldSize - (OldSize ? 1 : 0);
			ElementType* const Destination = RawData + OldEnd;
			ConstructElements<ElementType>(Destination, InString, InSize);

			ElementType* const NewEnd = RawData + GetSize();
			*NewEnd = '\0';

			return *this;
		}

		FORCEINLINE TString& AppendChars(const ElementType* String)
		{
			return AppendChars(String, String::GetSize(String));
		}

		FORCEINLINE TString& Append(const ElementType* String)
		{
			return AppendChars(String);
		}

		template<IsCharRange T>
		FORCEINLINE TString& Append(T&& Range)
		{
			const ElementType* RangeData = Bloodshot::GetData(Range);
			const size_t RangeSize = Bloodshot::GetSize(std::forward<T>(Range));
			return AppendChars(RangeData, RangeSize);
		}

		FORCEINLINE void RemoveAt(const size_t Index, const size_t Count)
		{
			RangeCheck(Index, Count);
			Data.RemoveAt(Index, Count);
		}

		FORCEINLINE void RemoveAt(const size_t Index)
		{
			RangeCheck(Index);
			Data.RemoveAt(Index);
		}

		FORCEINLINE void RemoveLast(const size_t Count = 1)
		{
			BS_CHECK(GetSize() >= Count);
			Data.RemoveAt(GetSize() - Count, Count);
		}

		NODISCARD FORCEINLINE size_t FindChar(const ElementType Char) const
		{
			const ElementType* const RawData = Data.GetData();
			for (size_t i = 0; i < GetSize(); ++i)
			{
				if (RawData[i] == Char)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool FindChar(const ElementType Char, size_t& OutIndex) const
		{
			OutIndex = FindChar(Char);
			return OutIndex != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE size_t FindLastChar(const ElementType Char) const
		{
			const ElementType* const RawData = GetData();
			for (size_t i = GetSize(); i > 0; --i)
			{
				if (RawData[i - 1] == Char)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool FindLastChar(const ElementType Char, size_t& OutIndex) const
		{
			OutIndex = FindLastChar(Char);
			return OutIndex != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE size_t Find(const ElementType* const SubStr) const
		{
			const ElementType* const RawData = GetData();
			const size_t SubStrSize = String::GetSize(SubStr);
			size_t SameCharCount = 0;
			for (size_t i = 0; i < SubStrSize; ++i)
			{
				if (RawData[i] == SubStr[i])
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

		NODISCARD FORCEINLINE bool Find(const ElementType* const SubStr, size_t& OutIndex) const
		{
			OutIndex = Find(SubStr);
			return OutIndex != IReservedValues::NoneIndex;
		}

		template<IsCharRange T>
		NODISCARD FORCEINLINE size_t Find(T&& Range) const
		{
			return Find(Bloodshot::GetData(std::forward<T>(Range)));
		}

		template<IsCharRange T>
		NODISCARD FORCEINLINE size_t Find(T&& Range, size_t& OutIndex) const
		{
			return Find(Bloodshot::GetData(std::forward<T>(Range)), OutIndex);
		}

		NODISCARD FORCEINLINE bool Contains(const ElementType* const SubStr) const
		{
			return Find(SubStr) != IReservedValues::NoneIndex;
		}

		template<IsCharRange T>
		NODISCARD FORCEINLINE bool Contains(T&& Range) const
		{
			return Find(std::forward<T>(Range)) != IReservedValues::NoneIndex;
		}

		FORCEINLINE size_t ParseIntoArray(TArray<TString>& OutArray, const ElementType* const InDelimiter)
		{
			BS_CHECK(InDelimiter);
			OutArray.Clear();
			if (InDelimiter[0] != '\0')
			{
				String::ParseTokens(*this, InDelimiter, [&OutArray](FStringView Visited)
				{
					if (Visited.GetSize())
					{
						OutArray.EmplaceBack(Visited);
					}
				});
			}
			return OutArray.GetSize();
		}

		using FIterator = DataType::FIterator;
		using FConstIterator = DataType::FConstIterator;

		using FRangeForIterator = DataType::FRangeForIterator;
		using FRangeForConstIterator = DataType::FRangeForConstIterator;

		NODISCARD FORCEINLINE FIterator CreateIterator()
		{
			return Data.CreateIterator();
		}

		NODISCARD FORCEINLINE FConstIterator CreateConstIterator() const
		{
			return Data.CreateConstIterator();
		}

		// For internal usage only!
		NODISCARD FORCEINLINE FRangeForIterator begin() { return Data.begin(); }
		NODISCARD FORCEINLINE FRangeForIterator end() { return Data.end(); }
		NODISCARD FORCEINLINE FRangeForConstIterator begin() const { return Data.begin(); }
		NODISCARD FORCEINLINE FRangeForConstIterator end() const { return Data.end(); }

	private:
		FORCEINLINE void RangeCheck(const size_t Index) const
		{
			BS_ASSERT(Index >= 0 && Index < GetSize(), "TString: Index out of bounds, Index - {}, Size - {}", Index, GetSize());
		}

		FORCEINLINE void RangeCheck(const size_t Index, const size_t Count) const
		{
			BS_ASSERT(Index >= 0 && Index + Count <= GetSize(), "TString: Index out of bounds, Index - {}, Count - {} Size - {}", Index, Count, GetSize());
		}
	};

	template<typename CharType>
	struct TIsZeroConstructible<TString<CharType>>
	{
		static constexpr bool Value = true;
	};

	NODISCARD FORCEINLINE uint64_t GetTypeHash(const FString& String)
	{
		return Private::TypeHash::GetFnv1aHashOfArrayRepresentation(String.GetData(), String.GetSize());
	}
}
