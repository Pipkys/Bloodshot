#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FVertexBuffer;
	class FIndexBuffer;

	class FVertexArray final
	{
		friend class FRHI;

	public:
		~FVertexArray();

		NODISCARD FORCEINLINE TArray<FVertexBuffer*>& GetVertexBuffers()
		{
			return VertexBuffers;
		}

		NODISCARD FORCEINLINE const TArray<FVertexBuffer*>& GetVertexBuffers() const
		{
			return VertexBuffers;
		}

		NODISCARD FORCEINLINE FIndexBuffer* GetIndexBuffer()
		{
			return IndexBuffer;
		}

		NODISCARD FORCEINLINE const FIndexBuffer* GetIndexBuffer() const
		{
			return IndexBuffer;
		}

		NODISCARD FORCEINLINE uint32_t GetVertexCount() const
		{
			return VertexCount;
		}

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(FVertexBuffer* const InVertexBuffer);
		void SetIndexBuffer(FIndexBuffer* const InIndexBuffer);

	private:
		FVertexArray();

		uint32_t UniqueID;

		TArray<FVertexBuffer*> VertexBuffers;
		FIndexBuffer* IndexBuffer = nullptr;

		uint32_t VertexBufferIndex = 0;
		uint32_t VertexBufferOffset = 0;
		uint32_t VertexBufferElementIndex = 0;
		uint32_t VertexCount = 0;
	};
}
