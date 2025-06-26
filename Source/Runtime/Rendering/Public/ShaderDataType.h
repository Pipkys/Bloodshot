#pragma once

#include "Core.h"

namespace Bloodshot
{
	enum class EShaderDataType : uint8_t
	{
		None = 0,
		Bool,
		Int,
		Int2,
		Int3,
		Int4,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4
	};

	NODISCARD FORCEINLINE constexpr uint8_t GetShaderDataTypeSize(const EShaderDataType Type)
	{
		switch (Type)
		{
			case EShaderDataType::Bool: return 1;
			case EShaderDataType::Int: return 4;
			case EShaderDataType::Int2: return 4 * 2;
			case EShaderDataType::Int3: return 4 * 3;
			case EShaderDataType::Int4: return 4 * 4;
			case EShaderDataType::Float: return 4;
			case EShaderDataType::Float2: return 4 * 2;
			case EShaderDataType::Float3: return 4 * 3;
			case EShaderDataType::Float4: return 4 * 4;
			case EShaderDataType::Mat3: return 4 * 3 * 3;
			case EShaderDataType::Mat4: return 4 * 4 * 4;
		}

		BS_ASSERT(false, "GetShaderDataTypeSize: Trying to get size from unknown ShaderDataType");
		return 0;
	}

	NODISCARD FORCEINLINE constexpr uint8_t GetShaderDataTypeElementCount(const EShaderDataType Type)
	{
		switch (Type)
		{
			case EShaderDataType::Bool: return 1;
			case EShaderDataType::Int: return 1;
			case EShaderDataType::Int2: return 2;
			case EShaderDataType::Int3: return 3;
			case EShaderDataType::Int4: return 4;
			case EShaderDataType::Float: return 1;
			case EShaderDataType::Float2: return 2;
			case EShaderDataType::Float3: return 3;
			case EShaderDataType::Float4: return 4;
			case EShaderDataType::Mat3: return 3;
			case EShaderDataType::Mat4: return 4;
		}

		BS_ASSERT(false, "GetShaderDataTypeElementCount: Trying to get element count from unknown ShaderDataType");
		return 0;
	}
}
