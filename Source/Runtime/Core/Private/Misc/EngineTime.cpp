#include "Misc/EngineTime.h"
#include "Profiling/ProfilingMacros.h"

namespace Bloodshot
{
	FEngineTime& FEngineTime::GetInstance()
	{
		static FEngineTime Instance;
		return Instance;
	}

	void FEngineTime::Tick()
	{
		BS_PROFILE_FUNCTION();
		FEngineTime& Instance = GetInstance();
		const std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
		const std::chrono::duration<float> Duration = Now - Instance.StartTimepoint;
		Instance.GlobalTime = Duration.count();
	}
}
