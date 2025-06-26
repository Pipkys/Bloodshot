#pragma once

#include "Allocators/Allocator.h"
#include "Misc/AssertionMacros.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	struct FStackLinearAllocatorStats final
	{
		size_t Size;
		size_t Offset;
	};

	template<size_t Size>
	class TStackLinearAllocator final : public IAllocator
	{
	public:
		class ForAnyElementType : public IAllocator::Contiguous::ForAnyElementType
		{
		public:
			NODISCARD FORCEINLINE void* GetAllocation() const
			{
				return (void*)Data;
			}

			NODISCARD FORCEINLINE size_t GetInitialCapacity() const
			{
				return Size;
			}

			NODISCARD FORCEINLINE bool HasAllocation() const
			{
				return true;
			}

			NODISCARD FORCEINLINE void* ExchangeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
				return nullptr;
			}

			FORCEINLINE void ConsumeAllocation(void* const InData) const
			{
			}

			FORCEINLINE void ResizeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
			}

			FORCEINLINE void Dispose()
			{
			}

		private:
			std::byte Data[Size];
			size_t Offset = 0;
		};

		template<typename ElementType>
		class ForElementType : public ForAnyElementType
		{
			using Super = ForAnyElementType;
			using Super::GetAllocation;
			using Super::ExchangeAllocation;

		public:
			NODISCARD FORCEINLINE ElementType* GetAllocation() const
			{
				return reinterpret_cast<ElementType*>(Super::GetAllocation());
			}

			NODISCARD FORCEINLINE ElementType* ExchangeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
				return reinterpret_cast<ElementType*>(Super::ExchangeAllocation(CurrentSize, NewSize, ElementSizeInBytes));
			}
		};
	};
}
