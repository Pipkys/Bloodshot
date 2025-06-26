#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FShader final
	{
		friend class FRHI;

	public:
		~FShader();

		NODISCARD FORCEINLINE uint32_t GetUniqueID() const
		{
			return UniqueID;
		}

		void Bind() const;
		void Unbind() const;

		void SetUniformInt(FStringView InName, const int InValue);
		void SetUniformFloat(FStringView InName, const float InValue);
		void SetUniformVec3(FStringView InName, const glm::vec3& InValue);
		void SetUniformVec4(FStringView InName, const glm::vec4& InValue);
		void SetUniformMat4(FStringView InName, const glm::mat4& InValue);

	protected:
		FShader(FStringView InName, FStringView InVertexShaderSrc, FStringView InFragmentShaderSrc);

		uint32_t UniqueID;
	};
}
