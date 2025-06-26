#pragma once

#ifdef BS_PROFILING_ON

#include "Misc/Timer.h"
#include "Platform/Platform.h"
#include "Profiling/Profiler.h"

#include <chrono>

namespace Bloodshot
{
	class FProfileTimer final : public FTimer
	{
	public:
		using Super = FTimer;

		FORCEINLINE FProfileTimer(const char* InFunctionName)
			: FunctionName(InFunctionName)
		{
		}

		FORCEINLINE ~FProfileTimer()
		{
			std::chrono::milliseconds Duration = Super::GetElapsedMilliseconds();
			FProfiler::WriteFunctionProfile(FunctionName, Duration);
		}

	private:
		const char* FunctionName;
	};
}

#endif
