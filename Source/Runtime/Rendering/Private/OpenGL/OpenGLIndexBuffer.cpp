#include "IndexBuffer.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	FIndexBuffer::FIndexBuffer(const void* const InIndices, const uint32_t InIndexCount)
		: IndexCount(InIndexCount)
	{
		glCreateBuffers(1, &UniqueID);
		glNamedBufferStorage(UniqueID, InIndexCount * sizeof(uint32_t), InIndices, GL_MAP_READ_BIT);
	}

	FIndexBuffer::~FIndexBuffer()
	{
		glDeleteBuffers(1, &UniqueID);
	}

	void FIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, UniqueID);
	}

	void FIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
