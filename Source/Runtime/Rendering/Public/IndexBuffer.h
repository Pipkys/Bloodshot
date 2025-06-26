#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FIndexBuffer final
	{
		friend class FRHI;

	public:
		~FIndexBuffer();

		void Bind() const;
		void Unbind() const;

		NODISCARD FORCEINLINE uint32_t GetUniqueID() const
		{
			return UniqueID;
		}

		NODISCARD FORCEINLINE size_t GetIndexCount() const
		{
			return IndexCount;
		}

	private:
		FIndexBuffer(const void* const InIndices, const uint32_t InIndexCount);

		uint32_t UniqueID;
		uint32_t IndexCount;
	};
}
