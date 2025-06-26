#pragma once

#include "Memory/Memory.h"
#include "String/LowLevelString.h"
#include "String/LowLevelStringAllocator.h"
#include "String/LowLevelStringBuilder.h"
#include "String/StringManipulations.h"
#include "Templates/MemoryOperations.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	namespace Private::String
	{
		template<typename ElementType>
		static void LLFormatHelper(FLowLevelString& OutStr, FLowLevelString& OutFmt, FLowLevelStringAllocator& OutAllocator, const ElementType& InElement)
		{
			char*& OutFmtData = OutFmt.Data;
			size_t& OutFmtSize = OutFmt.Size;

			char* const OutStrData = OutStr.Data;
			size_t& OutStrSize = OutStr.Size;

			size_t FmtLengthToAppend;
			size_t SecondBracketIndex;

			size_t Cursor = 0;
			const size_t SizeToInspect = OutFmtSize;
			while (Cursor < SizeToInspect)
			{
				bool FoundBracketCombo = false;
				if (OutFmtData[Cursor] == '}')
				{
					SecondBracketIndex = Cursor + 1;
					BS_ASSERT(SecondBracketIndex < OutFmtSize && OutFmtData[SecondBracketIndex] == '}',
						"LLFormatHelper: Incorrect format passed");
					FmtLengthToAppend = Cursor + 1;
					OutAllocator.Allocate(FmtLengthToAppend);
					MoveConstructElements(OutStrData + OutStrSize, OutFmtData, FmtLengthToAppend);
					FoundBracketCombo = true;
				}
				else if (OutFmtData[Cursor] == '{')
				{
					SecondBracketIndex = Cursor + 1;
					BS_ASSERT(SecondBracketIndex < OutFmtSize 
						&& (OutFmtData[SecondBracketIndex] == '}' || OutFmtData[SecondBracketIndex] == '{'),
						"LLFormatHelper: Incorrect format passed");
					if (OutFmtData[SecondBracketIndex] == '}')
					{
						FmtLengthToAppend = Cursor;
						OutAllocator.Allocate(FmtLengthToAppend);
						MoveConstructElements(OutStrData + OutStrSize, OutFmtData, FmtLengthToAppend);
						OutStrSize += FmtLengthToAppend;
						FLowLevelString ElementStrData = TLowLevelStringBuilder<ElementType>::Build(InElement, OutAllocator);
						OutStrSize += ElementStrData.Size;
						OutFmtData += SecondBracketIndex + 1;
						OutFmtSize -= SecondBracketIndex + 1;
						break;
					}
					else if (OutFmtData[SecondBracketIndex] == '{')
					{
						FmtLengthToAppend = Cursor + 1;
						OutAllocator.Allocate(FmtLengthToAppend);
						MoveConstructElements(OutStrData + OutStrSize, OutFmtData, FmtLengthToAppend);
						FoundBracketCombo = true;
					}
				}
				if (FoundBracketCombo)
				{
					OutStrSize += FmtLengthToAppend;
					OutFmtData += SecondBracketIndex + 1;
					OutFmtSize -= SecondBracketIndex + 1;
					Cursor = 0;
				}
				else
				{
					++Cursor;
				}
			}
		}

		static void LLFormatHelper(FLowLevelString& OutStr, FLowLevelString& OutFmt, FLowLevelStringAllocator& OutAllocator)
		{
			char*& OutFmtData = OutFmt.Data;
			size_t& OutFmtSize = OutFmt.Size;

			char* const OutStrData = OutStr.Data;
			size_t& OutStrSize = OutStr.Size;

			size_t FmtLengthToAppend;
			size_t SecondBracketIndex;

			size_t Cursor = 0;
			while (Cursor < OutFmtSize)
			{
				bool FoundBracketCombo = false;
				if (OutFmtData[Cursor] == '}')
				{
					SecondBracketIndex = Cursor + 1;
					BS_ASSERT(SecondBracketIndex < OutFmtSize && OutFmtData[SecondBracketIndex] == '}',
						"LLFormatHelper: Incorrect format passed");
					FmtLengthToAppend = Cursor + 1;
					FoundBracketCombo = true;
				}
				else if (OutFmtData[Cursor] == '{')
				{
					SecondBracketIndex = Cursor + 1;
					BS_ASSERT(SecondBracketIndex < OutFmtSize
						&& (OutFmtData[SecondBracketIndex] == '}' || OutFmtData[SecondBracketIndex] == '{'),
						"LLFormatHelper: Incorrect format passed");
					if (OutFmtData[SecondBracketIndex] == '}')
					{
						FmtLengthToAppend = Cursor;
						FoundBracketCombo = true;
					}
					else if (OutFmtData[SecondBracketIndex] == '{')
					{
						FmtLengthToAppend = Cursor + 1;
						FoundBracketCombo = true;
					}
				}
				if (FoundBracketCombo)
				{
					OutAllocator.Allocate(FmtLengthToAppend);
					MoveConstructElements(OutStrData + OutStrSize, OutFmtData, FmtLengthToAppend);
					OutStrSize += FmtLengthToAppend;
					OutFmtData += SecondBracketIndex + 1;
					OutFmtSize -= SecondBracketIndex + 1;
					++Cursor;
				}
				++Cursor;
			}
		}

		template<typename... ArgTypes>
		NODISCARD FLowLevelString LLFormatImpl(const char* InFmt, ArgTypes&&... Args)
		{
			static FLowLevelStringAllocator Allocator;
			FLowLevelString Fmt;
			const size_t& FmtSize = Fmt.Size = Bloodshot::String::GetSize(InFmt);
			char* const& FmtData = Fmt.Data = Allocator.Allocate(FmtSize);
			ConstructElements<char>(FmtData, InFmt, FmtSize);
			FLowLevelString Result;
			char* const ResultData = Allocator.Allocate(0);
			Result.Data = ResultData;
			size_t& ResultSize = Result.Size;
			(LLFormatHelper(Result, Fmt, Allocator, std::forward<ArgTypes>(Args)), ...);
			if constexpr (sizeof...(Args) > 0)
			{
				LLFormatHelper(Result, Fmt, Allocator);
			}
			MoveConstructElements(ResultData + ResultSize, FmtData, FmtSize);
			ResultSize += FmtSize + 1;
			ResultData[ResultSize - 1] = '\0';
			Allocator.Reset();
			return Result;
		}

		template<typename... ArgTypes>
		NODISCARD size_t LLFormatImpl(const char* InFmt, char* const OutBuffer, ArgTypes&&... Args)
		{
			static FLowLevelStringAllocator Allocator;
			FLowLevelString Fmt;
			const size_t& FmtSize = Fmt.Size = Bloodshot::String::GetSize(InFmt);
			char* const& FmtData = Fmt.Data = Allocator.Allocate(FmtSize);
			ConstructElements<char>(FmtData, InFmt, FmtSize);
			FLowLevelString Result;
			char* const ResultData = Allocator.Allocate(0);
			Result.Data = ResultData;
			size_t& ResultSize = Result.Size;
			(LLFormatHelper(Result, Fmt, Allocator, std::forward<ArgTypes>(Args)), ...);
			if constexpr (sizeof...(Args) > 0)
			{
				LLFormatHelper(Result, Fmt, Allocator);
			}
			MoveConstructElements(ResultData + ResultSize, FmtData, FmtSize);
			ResultSize += FmtSize + 1;
			ResultData[ResultSize - 1] = '\0';
			Allocator.Reset();
			memcpy(OutBuffer, ResultData, ResultSize);
			return ResultSize;
		}

		template<typename... ArgTypes>
		NODISCARD FLowLevelString LLFormat(const char* const InFmt, ArgTypes&&... Args)
		{
			FLowLevelString Temp = LLFormatImpl(InFmt, std::forward<ArgTypes>(Args)...);
			const size_t TempSize = Temp.Size;
			FLowLevelString Result;
			char* const ResultData = Result.Allocate(TempSize);
			MoveConstructElements(ResultData, Temp.Data, TempSize);
			--Result.Size;
			return Result;
		}
	}
}
