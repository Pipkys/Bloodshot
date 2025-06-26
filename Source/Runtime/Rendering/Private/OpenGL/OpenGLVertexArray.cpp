#include "VertexArray.h"
#include "IndexBuffer.h"
#include "OpenGL/OpenGLHeader.h"
#include "OpenGL/ShaderDataTypeToOpenGLBaseType.h"
#include "Vertex.h"
#include "VertexBuffer.h"

namespace Bloodshot
{
	FVertexArray::FVertexArray()
	{
		glCreateVertexArrays(1, &UniqueID);
	}

	FVertexArray::~FVertexArray()
	{
		glDeleteVertexArrays(1, &UniqueID);
	}

	void FVertexArray::Bind() const
	{
		glBindVertexArray(UniqueID);
	}

	void FVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void FVertexArray::AddVertexBuffer(FVertexBuffer* const InVertexBuffer)
	{
		glVertexArrayVertexBuffer(UniqueID,
			VertexBufferIndex,
			InVertexBuffer->GetUniqueID(),
			VertexBufferOffset,
			sizeof(FVertex));
		
		VertexBufferOffset += InVertexBuffer->GetVertexCount() * sizeof(FVertex);

		const FVertexBufferLayout& VertexBufferLayout = InVertexBuffer->GetLayout();
		BS_CHECK(VertexBufferLayout.GetSize());

		using FElementConstIterator = FVertexBufferLayout::FElementArray::FConstIterator;
		for (FElementConstIterator ElementIt = VertexBufferLayout.CreateConstIterator(); ElementIt; ++ElementIt)
		{
			const EShaderDataType ShaderDataType = ElementIt->Type;
			const uint8_t ShaderElementCount = GetShaderDataTypeElementCount(ShaderDataType);
			const GLenum ShaderOpenGLType = ShaderDataTypeToOpenGLBaseType(ShaderDataType);
			const GLboolean bElementNormalized = ElementIt->bNormalized ? GL_TRUE : GL_FALSE;
			const uint16_t LayoutStride = VertexBufferLayout.GetStride();
			const uint16_t ElementOffset = ElementIt->Offset;

			switch (ShaderDataType)
			{
				case EShaderDataType::Bool: FALLTHROUGH;
				case EShaderDataType::Int: FALLTHROUGH;
				case EShaderDataType::Int2: FALLTHROUGH;
				case EShaderDataType::Int3: FALLTHROUGH;
				case EShaderDataType::Int4: FALLTHROUGH;
				case EShaderDataType::Float: FALLTHROUGH;
				case EShaderDataType::Float2: FALLTHROUGH;
				case EShaderDataType::Float3: FALLTHROUGH;
				case EShaderDataType::Float4:
				{
					glEnableVertexArrayAttrib(UniqueID, VertexBufferElementIndex);
					glVertexArrayAttribFormat(UniqueID,
						VertexBufferElementIndex,
						ShaderElementCount,
						ShaderOpenGLType,
						bElementNormalized,
						ElementOffset);
					glVertexArrayAttribBinding(UniqueID, VertexBufferElementIndex++, VertexBufferIndex);
					continue;
				}
				case EShaderDataType::Mat3: FALLTHROUGH;
				case EShaderDataType::Mat4:
				{
					for (uint8_t i = 0; i < ShaderElementCount; ++i)
					{
						glEnableVertexArrayAttrib(UniqueID, VertexBufferElementIndex);
						glVertexArrayAttribFormat(UniqueID,
							VertexBufferElementIndex,
							ShaderElementCount,
							ShaderOpenGLType,
							bElementNormalized,
							ElementOffset + (uint16_t)((uint8_t)sizeof(float) * ShaderElementCount * i));
						glVertexArrayAttribBinding(UniqueID, VertexBufferElementIndex, VertexBufferIndex);
						glVertexArrayBindingDivisor(UniqueID, VertexBufferElementIndex++, 1);
					}
					continue;
				}
				default:
				{
					BS_ASSERT(false, "Trying to add FVertexBufferElement with unknown EShaderDataType to VertexArray");
					break;
				}
			}
		}

		VertexCount += VertexBuffers.EmplaceBackGetRef(InVertexBuffer)->GetVertexCount();
		++VertexBufferIndex;
	}

	void FVertexArray::SetIndexBuffer(FIndexBuffer* const InIndexBuffer)
	{
		IndexBuffer = InIndexBuffer;
		glVertexArrayElementBuffer(UniqueID, IndexBuffer->GetUniqueID());
	}
}
