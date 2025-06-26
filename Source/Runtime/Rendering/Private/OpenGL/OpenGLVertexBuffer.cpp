#include "VertexBuffer.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	namespace Private::OpenGL::VertexBuffer
	{
		FORCEINLINE void CreateBuffer(uint32_t& OutUniqueID, const void* const InVertices, const size_t InSize)
		{
			glCreateBuffers(1, &OutUniqueID);
			glNamedBufferStorage(OutUniqueID, InSize, InVertices, GL_MAP_READ_BIT);
		}
	}

	FVertexBuffer::FVertexBuffer(const void* const InVertices,
		const uint32_t InVertexCount,
		const size_t InSize,
		const FVertexBufferLayout& InLayout)
		: VertexCount(InVertexCount)
		, Layout(InLayout)
	{
		Private::OpenGL::VertexBuffer::CreateBuffer(UniqueID, InVertices, InSize);
	}

	FVertexBuffer::FVertexBuffer(const void* const InVertices,
		const uint32_t InVertexCount,
		const size_t InSize,
		FVertexBufferLayout&& InLayout)
		: VertexCount(InVertexCount)
		, Layout(std::move(InLayout))
	{
		Private::OpenGL::VertexBuffer::CreateBuffer(UniqueID, InVertices, InSize);
	}

	FVertexBuffer::~FVertexBuffer()
	{
		glDeleteBuffers(1, &UniqueID);
	}

	void FVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, UniqueID);
	}

	void FVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//void FVertexBuffer::SetVertices(const void* const InVertices, const size_t InSize)
	//{
	//	glNamedBufferSubData(UniqueID, 0, InSize, InVertices);
	//}
}
