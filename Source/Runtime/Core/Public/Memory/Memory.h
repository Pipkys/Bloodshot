#pragma once

#include "Platform/Platform.h"

#include <cstdint>

namespace Bloodshot
{
	struct FAllocationsInfo final
	{
		size_t AllocatedSize = 0;
		size_t AllocatedBlockCount = 0;
		size_t DeallocatedSize = 0;
		size_t DeallocatedBlockCount = 0;
	};

	struct FCurrentMemoryUsageInfo final
	{
		size_t Size = 0;
		size_t BlockCount = 0;
	};

	class FAllocationLogger final
	{
		friend class FMemory;

	public:
		NODISCARD static FAllocationLogger& GetInstance();

		NODISCARD FORCEINLINE static const FAllocationsInfo& GetAllocationsInfo()
		{
			return GetInstance().AllocationsInfo;
		}

		NODISCARD FORCEINLINE static const FCurrentMemoryUsageInfo& GetCurrentMemoryUsageInfo()
		{
			return GetInstance().CurrentMemoryUsageInfo;
		}

		NODISCARD FORCEINLINE static bool IsMemoryLeak()
		{
			const FAllocationsInfo& AllocationsInfo = GetAllocationsInfo();
			return (AllocationsInfo.AllocatedSize != AllocationsInfo.DeallocatedSize)
				|| (AllocationsInfo.AllocatedBlockCount != AllocationsInfo.DeallocatedBlockCount);
		}

	private:
		FAllocationLogger() {}

		FAllocationsInfo AllocationsInfo;
		FCurrentMemoryUsageInfo CurrentMemoryUsageInfo;

		FORCEINLINE static void OnMemoryAllocated(const size_t Size)
		{
			FAllocationLogger& Instance = GetInstance();

			FAllocationsInfo& AllocationsInfo = Instance.AllocationsInfo;
			AllocationsInfo.AllocatedSize += Size;
			++AllocationsInfo.AllocatedBlockCount;

			FCurrentMemoryUsageInfo& CurrentMemoryUsageInfo = Instance.CurrentMemoryUsageInfo;
			CurrentMemoryUsageInfo.Size += Size;
			++CurrentMemoryUsageInfo.BlockCount;
		}

		FORCEINLINE static void OnMemoryDeallocated(const size_t Size)
		{
			FAllocationLogger& Instance = GetInstance();

			FAllocationsInfo& AllocationInfo = Instance.AllocationsInfo;
			AllocationInfo.DeallocatedSize += Size;
			++AllocationInfo.DeallocatedBlockCount;

			FCurrentMemoryUsageInfo& CurrentMemoryUsageInfo = Instance.CurrentMemoryUsageInfo;
			CurrentMemoryUsageInfo.Size -= Size;
			--CurrentMemoryUsageInfo.BlockCount;
		}
	};

	enum class EAllocationType : uint8_t
	{
		Dynamic = 0,
		Temporary
	};

	class FMemory final
	{
	public:
		NODISCARD static FMemory& GetInstance();

		NODISCARD static void* Malloc(const size_t Size);
		static void Free(void* const Block);
		NODISCARD static void* Realloc(void* const Block, const size_t Size);

		NODISCARD static void* Allocate(const size_t Size, const EAllocationType AllocationType = EAllocationType::Dynamic);
		static void Deallocate(void* const Block);

	private:
		FMemory() {}
	};
}
