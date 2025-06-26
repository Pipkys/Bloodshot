#include "WorldRenderer.h"

namespace Bloodshot
{
	FWorldRenderer& FWorldRenderer::GetInstance()
	{
		static FWorldRenderer Instance;
		return Instance;
	}
}
