#pragma once

#include "Core.h"

#include "Asset.h"
#include "MaterialAsset.h"
#include "RHI.h"
#include "SourceMeshDescription.h"
#include "Vertex.h"

namespace Bloodshot
{
	class FVertexArray;

	BSCLASS();
	class FStaticMeshAsset final : public IObject
	{
		GENERATED_BODY();

	public:
		virtual ~FStaticMeshAsset() override
		{
			FRHI::DestroyVertexArray(VertexArray);
		}

		BSPROPERTY();
		TArray<FVertex> Vertices;

		BSPROPERTY();
		TArray<uint32_t> Indices;

		BSPROPERTY();
		TArray<FSourceMeshDescription> SourceMeshes;

		BSPROPERTY();
		TArray<TAssetSharedPtr<FMaterialAsset>> MaterialAssets;

		FVertexArray* VertexArray = nullptr;
	};
}
