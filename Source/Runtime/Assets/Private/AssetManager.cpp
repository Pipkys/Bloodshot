#include "AssetManager.h"

namespace Bloodshot
{
	FAssetManager& FAssetManager::GetInstance()
	{
		static FAssetManager Instance;
		return Instance;
	}
}
