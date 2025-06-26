#pragma once

#include "Core.h"

#include "ShaderDataType.h"

namespace Bloodshot
{
	struct FVertexBufferElement final
	{
		FVertexBufferElement(const char* InName, const EShaderDataType InType, const bool bInNormalized = false)
			: Name(InName)
			, Type(InType)
			, Size(GetShaderDataTypeSize(InType))
			, bNormalized(bInNormalized)
		{
		}

		const char* Name = "";
		EShaderDataType Type = EShaderDataType::None;

		uint16_t Size = 0;
		uint16_t Offset = 0;

		bool bNormalized = false;
	};
}
