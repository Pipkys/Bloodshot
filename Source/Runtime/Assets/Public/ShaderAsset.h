#pragma once

#include "Core.h"

#include "RHI.h"
#include "Shader.h"

namespace Bloodshot
{
	BSCLASS();
	class FShaderAsset final : public IObject
	{
		GENERATED_BODY();

	public:
		virtual ~FShaderAsset() override
		{
			FRHI::DestroyShader(SourceShader);
		}

		FShader* SourceShader = nullptr;

		NODISCARD FORCEINLINE uint32_t GetSourceShaderUniqueID() const
		{
			return SourceShader->GetUniqueID();
		}

		FORCEINLINE void Bind() const
		{
			SourceShader->Bind();
		}

		FORCEINLINE void Unbind() const
		{
			SourceShader->Unbind();
		}

		FORCEINLINE void SetUniformInt(FStringView InName, const int InValue)
		{
			SourceShader->SetUniformInt(InName, InValue);
		}

		FORCEINLINE void SetUniformFloat(FStringView InName, const float InValue)
		{
			SourceShader->SetUniformFloat(InName, InValue);
		}

		FORCEINLINE void SetUniformVec3(FStringView InName, const glm::vec3& InValue)
		{
			SourceShader->SetUniformVec3(InName, InValue);
		}

		FORCEINLINE void SetUniformVec4(FStringView InName, const glm::vec4& InValue)
		{
			SourceShader->SetUniformVec4(InName, InValue);
		}

		FORCEINLINE void SetUniformMat4(FStringView InName, const glm::mat4& InValue)
		{
			SourceShader->SetUniformMat4(InName, InValue);
		}
	};
}
