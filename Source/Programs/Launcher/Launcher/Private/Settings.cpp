#include "Settings.h"

namespace Bloodshot::Launcher
{
	FSettings& FSettings::GetInstance()
	{
		static FSettings Instance;
		return Instance;
	}
}
