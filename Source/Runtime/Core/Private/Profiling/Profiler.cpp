#ifdef BS_PROFILING_ON

#include "Containers/Pair.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Misc/AssertionMacros.h"
#include "Platform/Platform.h"
#include "Profiling/Profiler.h"
#include "String/LowLevelFormat.h"
#include "String/LowLevelString.h"
#include "String/LowLevelStringAllocator.h"
#include "String/LowLevelStringBuilder.h"

#include <filesystem>

namespace Bloodshot
{
	namespace Private::String
	{
		template<>
		struct TLowLevelStringBuilder<std::chrono::milliseconds>
		{
			static FLowLevelString Build(const std::chrono::milliseconds& InMilli, FLowLevelStringAllocator& OutAllocator)
			{
				FLowLevelString IntPart = TLowLevelStringBuilder<long long>::Build(InMilli.count(), OutAllocator);
				FLowLevelString SuffixPart;
				SuffixPart.Data = OutAllocator.Allocate(2);
				SuffixPart.Size = 2;
				memcpy(SuffixPart.Data, "ms", 2);
				return FLowLevelString(IntPart.Data, IntPart.Size + SuffixPart.Size);
			}
		};
	}

	FProfiler& FProfiler::GetInstance()
	{
		static FProfiler Instance;
		return Instance;
	}

	void FProfiler::WriteFunctionProfile(const char* Name, const std::chrono::milliseconds Duration)
	{
		if (!IsSessionStarted())
		{
			return;
		}

		static size_t FunctionUniqueID = 0;
		FFunctionProfileMap& FunctionProfiles = GetInstance().FunctionProfiles;
		for (TPair<size_t, FFunctionProfile>& ProfilePair : FunctionProfiles)
		{
			FFunctionProfile& Profile = ProfilePair.Second;

			if (Profile.Name == Name)
			{
				++Profile.TotalExecutions;
				Profile.TotalExecutionDuration += Duration;
				return;
			}
		}

		FFunctionProfile Profile;
		Profile.Name = Name;
		Profile.TotalExecutions = 1;
		Profile.TotalExecutionDuration = Duration;
		FunctionProfiles.Emplace(++FunctionUniqueID, std::move(Profile));
	}

	void FProfiler::BeginSession()
	{
		FProfiler& Instance = GetInstance();
		bool& bSessionStarted = Instance.bSessionStarted;
		BS_CHECK(!bSessionStarted);
		if (!std::filesystem::exists("Logs"))
		{
			std::filesystem::create_directory("Logs");
		}

		std::ofstream& OutputStream = Instance.OutputStream;
		OutputStream.open("Logs/Profiler.txt");
		BS_CHECK(OutputStream.is_open());
		bSessionStarted = true;
	}

	void FProfiler::EndSession()
	{
		using std::chrono::milliseconds;
		using std::chrono::microseconds;

		FProfiler& Instance = GetInstance();
		std::ofstream& OutputStream = Instance.OutputStream;
		FFunctionProfileMap& FunctionProfiles = Instance.FunctionProfiles;

		BS_CHECK(Instance.bSessionStarted);
		OutputStream << "[-] - Not sorted:\n";

		for (TPair<size_t, FFunctionProfile>& ProfilePair : FunctionProfiles)
		{
			const size_t ProfileUniqueID = ProfilePair.First;
			FFunctionProfile& Profile = ProfilePair.Second;

			const size_t TotalExecutions = Profile.TotalExecutions;
			const milliseconds TotalExecutionDurationMilli = Profile.TotalExecutionDuration;
			const microseconds TotalExecutionDurationMicro = std::chrono::duration_cast<microseconds>(TotalExecutionDurationMilli);
			const float AverageExecutionDurationMilli = (float)TotalExecutionDurationMilli.count() / (float)TotalExecutions;
			const float AverageExecutionDurationMicro = (float)TotalExecutionDurationMicro.count() / (float)TotalExecutions;
			Profile.AverageExecutionDurationMilli = AverageExecutionDurationMilli;

			Private::String::FLowLevelString Result = Private::String::LLFormat("\t[{}] - [{}]: calls: {}; total: {}; average: {}ms / {}us\n\n",
				ProfileUniqueID,
				Profile.Name,
				TotalExecutions,
				TotalExecutionDurationMilli,
				AverageExecutionDurationMilli,
				AverageExecutionDurationMicro);
			OutputStream << Result.Data;
			Result.Release();
		}

		TArray<TPair<size_t, FFunctionProfile>> SortedByMs = FunctionProfiles.ToArray();
		// BSTODO: Implement Sort() ?
		//SortedByMs.Sort();
		std::vector<TPair<size_t, FFunctionProfile>> SortedByMsReal;
		for (TPair<size_t, FFunctionProfile>& Pair : SortedByMs)
		{
			SortedByMsReal.push_back(Pair);
		}
		//std::sort(SortedByMsReal.begin(), SortedByMsReal.end(), [](const TPair<size_t, FFunctionProfile>& Left, const TPair<size_t, FFunctionProfile>& Right)
		//{
		//	return Left.Second.AverageExecutionDurationMilli >= Right.Second.AverageExecutionDurationMilli;
		//});

		OutputStream << "[-] - Sorted by average in ms:\n";

		for (const TPair<size_t, FFunctionProfile>& ProfilePair : SortedByMsReal)
		{
			const size_t ProfileUniqueID = ProfilePair.First;
			const FFunctionProfile& Profile = ProfilePair.Second;
			const size_t TotalExecutions = Profile.TotalExecutions;
			const milliseconds TotalExecutionDurationMilli = Profile.TotalExecutionDuration;
			const float AverageExecutionDurationMilli = Profile.AverageExecutionDurationMilli;

			// BSTODO: Paddings
			Private::String::FLowLevelString Result = Private::String::LLFormat("\t[{}] - [{}]: calls: {}; total: {}; average: {}ms / {}us\n\n",
				ProfileUniqueID,
				Profile.Name,
				TotalExecutions,
				TotalExecutionDurationMilli,
				AverageExecutionDurationMilli,
				AverageExecutionDurationMilli * 1000.f);
			OutputStream << Result.Data;
			Result.Release();
		}

		FunctionProfiles.Dispose();
		OutputStream.close();
	}
}

#endif
