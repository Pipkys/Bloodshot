#pragma once

#include "Core.h"

#include "OpenGL/OpenGLHeader.h"
#include "ShaderDataType.h"

namespace Bloodshot
{
	NODISCARD FORCEINLINE constexpr GLenum ShaderDataTypeToOpenGLBaseType(const EShaderDataType Type)
	{
		switch (Type)
		{
			case EShaderDataType::Bool: return GL_BOOL;
			case EShaderDataType::Int: return GL_INT;
			case EShaderDataType::Int2: return GL_INT;
			case EShaderDataType::Int3: return GL_INT;
			case EShaderDataType::Int4: return GL_INT;
			case EShaderDataType::Float: return GL_FLOAT;
			case EShaderDataType::Float2: return GL_FLOAT;
			case EShaderDataType::Float3: return GL_FLOAT;
			case EShaderDataType::Float4: return GL_FLOAT;
			case EShaderDataType::Mat3: return GL_FLOAT;
			case EShaderDataType::Mat4: return GL_FLOAT;
		}

		BS_ASSERT(false, "ShaderDataTypeToOpenGLBaseType: Trying to convert unknown ShaderDataType to OpenGLBaseType");
		return 0;
	}
}
