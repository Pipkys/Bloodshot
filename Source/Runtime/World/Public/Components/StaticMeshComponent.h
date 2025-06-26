#pragma once

#include "Core.h"

#include "Asset.h"
#include "AssetManager.h"
#include "Component.h"
#include "MaterialAsset.h"
#include "StaticMeshAsset.h"

namespace Bloodshot
{
	BSCLASS();
	class FStaticMeshComponent final : public IComponent
	{
		GENERATED_BODY();

	public:
		FORCEINLINE FStaticMeshComponent(TAssetSharedPtr<FStaticMeshAsset> InStaticMeshAsset,
			TArray<TAssetSharedPtr<FMaterialAsset>>&& InMaterialAssets)
			: StaticMeshAsset(InStaticMeshAsset)
			, MaterialAssets(std::move(InMaterialAssets))
		{
		}

		FORCEINLINE FStaticMeshComponent(TAssetSharedPtr<FStaticMeshAsset> InStaticMeshAsset,
			const TArray<TAssetSharedPtr<FMaterialAsset>>& InMaterialAssets)
			: StaticMeshAsset(InStaticMeshAsset)
			, MaterialAssets(InMaterialAssets)
		{
		}

		FORCEINLINE FStaticMeshComponent(TAssetSharedPtr<FStaticMeshAsset> InStaticMeshAsset)
			: StaticMeshAsset(InStaticMeshAsset)
			, MaterialAssets(InStaticMeshAsset->MaterialAssets)
		{
		}

		BSPROPERTY();
		TAssetSharedPtr<FStaticMeshAsset> StaticMeshAsset;

		BSPROPERTY();
		TArray<TAssetSharedPtr<FMaterialAsset>> MaterialAssets;
	};
}
