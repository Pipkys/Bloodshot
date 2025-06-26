#pragma once

#include "Core.h"

#include "VertexBufferElement.h"

namespace Bloodshot
{
	class FVertexBufferLayout final
	{
	public:
		using FElementArray = TArray<FVertexBufferElement>;

		FVertexBufferLayout(std::initializer_list<FVertexBufferElement> InElements)
			: Elements(InElements)
		{
			CalculateElementsOffsetsAndStride();
		}

		NODISCARD FORCEINLINE const FElementArray& GetElements() const
		{
			return Elements;
		}

		NODISCARD FORCEINLINE size_t GetSize() const
		{
			return Elements.GetSize();
		}

		NODISCARD FORCEINLINE uint16_t GetStride() const
		{
			return Stride;
		}

		NODISCARD FORCEINLINE FElementArray::FIterator CreateIterator()
		{
			return Elements.CreateIterator();
		}

		NODISCARD FORCEINLINE FElementArray::FConstIterator CreateConstIterator() const
		{
			return Elements.CreateConstIterator();
		}

	private:
		FElementArray Elements;
		uint16_t Stride = 0;

		FORCEINLINE void CalculateElementsOffsetsAndStride()
		{
			uint16_t Offset = 0;
			Stride = 0;
			for (FVertexBufferElement& Element : Elements)
			{
				Element.Offset = Offset;
				Offset += Element.Size;
				Stride += Element.Size;
			}
		}
	};
}