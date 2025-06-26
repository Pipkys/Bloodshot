#pragma once

#ifdef BS_PROFILING_ON

#include "Containers/Map.h"
#include "Platform/Platform.h"

#include <chrono>
#include <fstream>

namespace Bloodshot
{
	class FProfiler final
	{
		friend struct Private::Launch::FDebugEntry;
		friend class FProfileTimer;

		struct FFunctionProfile final
		{
			const char* Name;
			size_t TotalExecutions;
			std::chrono::milliseconds TotalExecutionDuration;
			float AverageExecutionDurationMilli;
		};

		FProfiler() {}

		std::ofstream OutputStream;

		using FFunctionProfileMap = TMap<size_t, FFunctionProfile>;
		FFunctionProfileMap FunctionProfiles;

		bool bSessionStarted = false;

		FORCEINLINE static void StaticConstructor()
		{
			BeginSession();
		}

		FORCEINLINE static void StaticDestructor()
		{
			EndSession();
		}

		NODISCARD static FProfiler& GetInstance();

		NODISCARD FORCEINLINE static bool IsSessionStarted()
		{
			return GetInstance().bSessionStarted;
		}

		static void BeginSession();
		static void EndSession();

		static void WriteFunctionProfile(const char* Name, const std::chrono::milliseconds Duration);
	};
}

#endif
