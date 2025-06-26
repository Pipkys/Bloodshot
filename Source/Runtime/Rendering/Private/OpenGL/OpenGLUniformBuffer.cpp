#include "UniformBuffer.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	FUniformBuffer::FUniformBuffer(const size_t InSize, const uint32_t InBinding)
	{
		glCreateBuffers(1, &UniqueID);
		glNamedBufferData(UniqueID, InSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, InBinding, UniqueID);
	}

	FUniformBuffer::FUniformBuffer(const size_t InSize,
		const uint32_t InBinding,
		const void* const InData,
		const uint32_t InOffset)
	{
		glCreateBuffers(1, &UniqueID);
		glNamedBufferData(UniqueID, InSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, InBinding, UniqueID);
		glNamedBufferSubData(UniqueID, InOffset, InSize, InData);
	}

	FUniformBuffer::~FUniformBuffer()
	{
		glDeleteBuffers(1, &UniqueID);
	}

	void FUniformBuffer::SetData(const void* const InData, const size_t InSize, const uint32_t InOffset)
	{
		glNamedBufferSubData(UniqueID, InOffset, InSize, InData);
	}
}
