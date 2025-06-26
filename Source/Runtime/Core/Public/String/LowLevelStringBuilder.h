#pragma once

#include "String/LowLevelString.h"
#include "String/LowLevelStringAllocator.h"
#include "String/StringManipulations.h"
#include "Templates/IsCharRange.h"
#include "Templates/IsCharType.h"
#include "Templates/MemoryOperations.h"
#include "Templates/Template.h"
#include "Templates/TypeTraits.h"

#include <cmath>

namespace Bloodshot::Private::String
{
	template<typename InElementType>
	struct TLowLevelStringBuilder
	{
		static FLowLevelString Build(const InElementType& InElement, FLowLevelStringAllocator& Allocator)
		{
			static_assert(TFalseType<InElementType>::value, "Proved specialization of TLowLevelStringBuilder for T");
		}
	};

	template<IsIntegral T>
	struct TLowLevelStringBuilder<T>
	{
		static FLowLevelString Build(const T& InInt, FLowLevelStringAllocator& OutAllocator)
		{
			constexpr size_t BufferSize = sizeof(T) * CHAR_BIT;
			char Buffer[BufferSize];
			const size_t OutSize = snprintf(Buffer, BufferSize, "%llu", (uint64_t)InInt);
			char* const Result = OutAllocator.Allocate(OutSize);
			memcpy(Result, Buffer, OutSize);
			return FLowLevelString(Result, OutSize);
		}
	};

	template<IsFloatingPoint T>
	struct TLowLevelStringBuilder<T>
	{
		static FLowLevelString Build(const T& InFloat, FLowLevelStringAllocator& OutAllocator)
		{
			constexpr size_t BufferSize = sizeof(T) * CHAR_BIT;
			char Buffer[BufferSize];
			const size_t OutSize = snprintf(Buffer, BufferSize, "%.8g", InFloat);
			char* const Result = OutAllocator.Allocate(OutSize);
			memcpy(Result, Buffer, OutSize);
			return FLowLevelString(Result, OutSize);
		}
	};

	template<IsCharType T>
	struct TLowLevelStringBuilder<T>
	{
		static FLowLevelString Build(const T InChar, FLowLevelStringAllocator& OutAllocator)
		{
			FLowLevelString Result;
			Result.Size = 1;
			Result.Data = OutAllocator.Allocate(Result.Size);
			MoveConstructElements(Result.Data, &InChar, Result.Size);

			return Result;
		}
	};

	template<IsCharType T>
	struct TLowLevelStringBuilder<const T>
	{
		static FLowLevelString Build(const T InChar, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T>::Build(InChar, OutAllocator);
		}
	};

	template<IsCharType T>
	struct TLowLevelStringBuilder<volatile T>
	{
		static FLowLevelString Build(const T InChar, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T>::Build(InChar, OutAllocator);
		}
	};

	template<IsCharType T>
	struct TLowLevelStringBuilder<const volatile T>
	{
		static FLowLevelString Build(const T InChar, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T>::Build(InChar, OutAllocator);
		}
	};

	template<>
	struct TLowLevelStringBuilder<char*>
	{
		static FLowLevelString Build(const char* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			FLowLevelString Result;
			Result.Size = Bloodshot::String::GetSize(InStr);
			Result.Data = OutAllocator.Allocate(Result.Size);
			MoveConstructElements(Result.Data, InStr, Result.Size);
			return Result;
		}
	};

	template<>
	struct TLowLevelStringBuilder<const char*>
	{
		static FLowLevelString Build(const char* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<char*>::Build(InStr, OutAllocator);
		}
	};

	template<>
	struct TLowLevelStringBuilder<volatile char*>
	{
		static FLowLevelString Build(volatile char* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<char*>::Build((char*)InStr, OutAllocator);
		}
	};

	template<>
	struct TLowLevelStringBuilder<const volatile char*>
	{
		static FLowLevelString Build(const volatile char* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<char*>::Build((char*)InStr, OutAllocator);
		}
	};

	template<IsCharRange T>
	struct TLowLevelStringBuilder<T>
	{
		static FLowLevelString Build(const T& InStr, FLowLevelStringAllocator& OutAllocator)
		{
			FLowLevelString Result;
			Result.Size = Bloodshot::GetSize(InStr);
			Result.Data = OutAllocator.Allocate(Result.Size);
			MoveConstructElements(Result.Data, Bloodshot::GetData(InStr), Result.Size);
			return Result;
		}
	};

	template<IsCharRange T>
	struct TLowLevelStringBuilder<const T>
	{
		static FLowLevelString Build(const T& InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T*>::Build(InStr, OutAllocator);
		}
	};

	template<IsCharRange T>
	struct TLowLevelStringBuilder<volatile T>
	{
		static FLowLevelString Build(const T& InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T*>::Build(InStr, OutAllocator);
		}
	};

	template<IsCharRange T>
	struct TLowLevelStringBuilder<const volatile T>
	{
		static FLowLevelString Build(const T& InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<T*>::Build(InStr, OutAllocator);
		}
	};

	template<>
	struct TLowLevelStringBuilder<void*>
	{
		static FLowLevelString Build(const void* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<char*>::Build((char*)InStr, OutAllocator);
		}
	};

	template<>
	struct TLowLevelStringBuilder<const void*>
	{
		static FLowLevelString Build(const void* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<void*>::Build((void*)InStr, OutAllocator);
		}
	};

	template<>
	struct TLowLevelStringBuilder<volatile void*>
	{
		static FLowLevelString Build(const volatile void* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<void*>::Build((void*)InStr, OutAllocator);
		}
	};

	template<>
	struct TLowLevelStringBuilder<const volatile void*>
	{
		static FLowLevelString Build(const volatile void* InStr, FLowLevelStringAllocator& OutAllocator)
		{
			return TLowLevelStringBuilder<void*>::Build((void*)InStr, OutAllocator);
		}
	};
}
