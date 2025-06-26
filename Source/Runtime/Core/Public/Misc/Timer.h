#pragma once

#include "Platform/Platform.h"

#include <chrono>

namespace Bloodshot
{
	class FTimer
	{
	private:
		template<typename In>
		NODISCARD FORCEINLINE In GetTimeSinceEpochIn(const std::chrono::high_resolution_clock::time_point& TimePoint) const
		{
			return std::chrono::time_point_cast<In>(TimePoint).time_since_epoch();
		}

		template<typename In>
		NODISCARD FORCEINLINE In GetElapsedTimeIn() const
		{
			using namespace std::chrono;
			const high_resolution_clock::time_point EndTimepoint = high_resolution_clock::now();
			const In StartTime = GetTimeSinceEpochIn<In>(StartTimepoint);
			const In EndTime = GetTimeSinceEpochIn<In>(EndTimepoint);
			return EndTime - StartTime;
		}

	public:
		NODISCARD FORCEINLINE std::chrono::seconds GetElapsedSeconds() const
		{
			return GetElapsedTimeIn<std::chrono::seconds>();
		}

		NODISCARD FORCEINLINE std::chrono::milliseconds GetElapsedMilliseconds() const
		{
			return GetElapsedTimeIn<std::chrono::milliseconds>();
		}

		NODISCARD FORCEINLINE std::chrono::microseconds GetElapsedMicroseconds() const
		{
			return GetElapsedTimeIn<std::chrono::microseconds>();
		}

		FORCEINLINE void Reset()
		{
			StartTimepoint = std::chrono::high_resolution_clock::now();
		}

	protected:
		std::chrono::time_point<std::chrono::high_resolution_clock> StartTimepoint = std::chrono::high_resolution_clock::now();
	};
}
