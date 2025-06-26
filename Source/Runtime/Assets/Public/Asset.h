#pragma once

#include "Core.h"

namespace Bloodshot
{
#ifdef BS_WITH_EDITOR
	class FTexture;
#endif

	template<IsObject InAssetType>
	using TAssetSharedPtr = TObjectSharedPtr<InAssetType, true>;

	class FAsset final
	{
		friend class FAssetManager;

	public:
		FString Name;
#ifdef BS_WITH_EDITOR
		TArray<FString> SourceFiles;
		FString Path;
#endif
		TAssetSharedPtr<IObject> Data;
#ifdef BS_WITH_EDITOR
		FStringView Type;
#endif

	private:
		FAsset() {}
	};
}
