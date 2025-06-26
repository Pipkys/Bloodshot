#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FUniformBuffer final
	{
		friend class FRHI;

	public:
		~FUniformBuffer();

		void SetData(const void* const InData, const size_t InSize, const uint32_t InOffset = 0);

	private:
		FUniformBuffer(const size_t InSize,
			const uint32_t InBinding);

		FUniformBuffer(const size_t InSize,
			const uint32_t InBinding,
			const void* const InData, 
			const uint32_t InOffset = 0);

		uint32_t UniqueID;
	};
}
