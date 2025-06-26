#pragma once

#include "Core.h"

#include "VertexBufferLayout.h"

namespace Bloodshot
{
	class FVertexBuffer final
	{
		friend class FRHI;

	public:
		~FVertexBuffer();

		NODISCARD FORCEINLINE uint32_t GetUniqueID() const
		{
			return UniqueID;
		}

		NODISCARD FORCEINLINE const FVertexBufferLayout& GetLayout() const
		{
			return Layout;
		}

		NODISCARD FORCEINLINE uint32_t GetVertexCount() const
		{
			return VertexCount;
		}

		void Bind() const;
		void Unbind() const;

		//void SetVertices(const void* const InVertices, const size_t InSize);

		FORCEINLINE void SetLayout(const FVertexBufferLayout& InLayout)
		{
			Layout = InLayout;
		}

		FORCEINLINE void SetLayout(FVertexBufferLayout&& InLayout)
		{
			Layout = std::move(InLayout);
		}

	private:
		FVertexBuffer(const void* const InVertices,
			const uint32_t InVertexCount,
			const size_t InSize,
			const FVertexBufferLayout& InLayout);

		FVertexBuffer(const void* const InVertices,
			const uint32_t InVertexCount,
			const size_t InSize,
			FVertexBufferLayout&& InLayout);

		uint32_t UniqueID;
		uint32_t VertexCount;
		FVertexBufferLayout Layout;
	};
}
