#pragma once

#include "Core.h"

#include "Asset.h"
#include "MaterialProperties.h"
#include "ShaderAsset.h"
#include "TextureAsset.h"

namespace Bloodshot
{
	BSCLASS();
	class FMaterialAsset final : public IObject
	{
		GENERATED_BODY();

	public:
		BSPROPERTY();
		TAssetSharedPtr<FShaderAsset> ShaderAsset;

		BSPROPERTY();
		FMaterialProperties Properties;

		BSPROPERTY();
		TArray<TAssetSharedPtr<FTextureAsset>> TextureAssets;

		FORCEINLINE void BindShader() const
		{
			ShaderAsset->Bind();
		}

		FORCEINLINE void BindTextures() const
		{
			for (const TAssetSharedPtr<FTextureAsset>& Texture : TextureAssets)
			{
				Texture->Bind();
			}
		}

		FORCEINLINE void Bind() const
		{
			BindShader();
			BindTextures();
		}

		FORCEINLINE void UnbindShader() const
		{
			ShaderAsset->Unbind();
		}

		FORCEINLINE void UnbindTextures() const
		{
			for (const TAssetSharedPtr<FTextureAsset>& Texture : TextureAssets)
			{
				Texture->Unbind();
			}
		}

		FORCEINLINE void Unbind() const
		{
			UnbindTextures();
			UnbindShader();
		}
	};
}
