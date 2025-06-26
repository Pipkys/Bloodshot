#pragma once

#include "Core.h"

#include "RHI.h"
#include "Texture.h"

namespace Bloodshot
{
	BSCLASS();
	class FTextureAsset final : public IObject
	{
		GENERATED_BODY();

	public:
		virtual ~FTextureAsset() override
		{
			FRHI::DestroyTexture(SourceTexture);
		}

		BSPROPERTY();
		FTexture* SourceTexture = nullptr;

		BSPROPERTY();
		ETextureUnit Unit = ETextureUnit::BaseColor;

		NODISCARD FORCEINLINE uint32_t GetSourceTextureUniqueID() const
		{
			return SourceTexture->GetUniqueID();
		}

		FORCEINLINE void Bind() const
		{
			if (SourceTexture)
			{
				SourceTexture->Bind(Unit);
			}
		}

		FORCEINLINE void Unbind() const
		{
			if (SourceTexture)
			{
				SourceTexture->Unbind(Unit);
			}
		}
	};
}
