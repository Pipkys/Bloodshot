#pragma once

#include "Core.h"

namespace Bloodshot
{
	struct FMeshSource;

	class FAssetImporter final
	{
	public:
		NODISCARD static FAssetImporter& GetInstance();

		static void ImportMesh(FMeshSource& OutMeshSource, FStringView InPath);
	};
}
