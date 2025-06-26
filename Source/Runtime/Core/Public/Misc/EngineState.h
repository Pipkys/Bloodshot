#pragma once

#include "Platform/Platform.h"

namespace Bloodshot
{
	class FEngineState final
	{
		friend class Private::Launch::IEngineContext;
		friend class FEditor;

	public:
		NODISCARD static FEngineState& GetInstance();

		NODISCARD FORCEINLINE static bool IsRunning()
		{
			return GetInstance().bRunning;
		}

		NODISCARD FORCEINLINE static bool IsSimulating()
		{
			return GetInstance().bSimulating;
		}

	private:
		FEngineState() {}

		bool bRunning = false;
		bool bSimulating = false;

		NODISCARD FORCEINLINE static void SetRunning(const bool bInRunning)
		{
			GetInstance().bRunning = bInRunning;
		}

		NODISCARD FORCEINLINE static void SetSimulating(const bool bInSimulating)
		{
			GetInstance().bSimulating = bInSimulating;
		}
	};
}
