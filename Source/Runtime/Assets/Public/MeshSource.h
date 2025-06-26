#pragma once

#include "Core.h"

#include "MaterialProperties.h"
#include "SourceMeshDescription.h"
#include "Texture.h"
#include "Vertex.h"

namespace Bloodshot
{
	struct FMaterialSource final
	{
		FString Name;
		FMaterialProperties Properties;
	};

	struct FTextureSource final
	{
		FString Name = "";
		FString SourcePath = "";
		ETextureUnit TextureUnit = ETextureUnit::BaseColor;
		FTexture* SourceTexture = nullptr;
	};

	struct FMeshSource final
	{
		TArray<FVertex> Vertices;
		TArray<uint32_t> Indices;
		TArray<FSourceMeshDescription> SourceMeshes;
		TArray<FMaterialSource> MaterialSources;
		TArray<FTextureSource> TextureSources;
	};
}
