#include "Launcher.h"

namespace Bloodshot::Launcher
{
	FLauncher& FLauncher::GetInstance()
	{
		static FLauncher Instance;
		return Instance;
	}
}
