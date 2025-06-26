#pragma once

#include "Logging/LogLevel.h"
#include "Memory/Memory.h"
#include "Misc/EnumClassFlags.h"
#include "Misc/LocalTime.h"
#include "Platform/Platform.h"
#include "Containers/String.h"
#include "String/Format.h"

#include <fstream>

namespace Bloodshot
{
	class FLogger final
	{
		friend struct Private::Launch::FDebugEntry;

	public:
		NODISCARD static FLogger& GetInstance();

		NODISCARD FORCEINLINE static ELogLevel GetLogLevelFlags()
		{
			return GetInstance().LogLevelFlags;
		}

		NODISCARD FORCEINLINE static void SetLogLevelFlags(const ELogLevel LogLevelFlags)
		{
			GetInstance().LogLevelFlags = LogLevelFlags;
		}

		NODISCARD FORCEINLINE static bool IsSessionStarted()
		{
			return GetInstance().bSessionStarted;
		}

		template<typename... ArgTypes>
		static void Log(const ELogLevel InLevel, const char* InFormat, ArgTypes&&... Args)
		{
#ifdef BS_LOGGING_ON
			FLogger& Instance = GetInstance();
			const FString& LogString = String::Format(InFormat, std::forward<ArgTypes>(Args)...);
			const char* const RawLogString = LogString.GetData();
			const char* const LogLevelString = Private::LogLevel::LogLevelToString(InLevel);
			const char* const LocalTimeString = ILocalTime::Now();
			bool bWriteToFile = false;

			if (EnumHasAllFlags(Instance.LogLevelFlags, InLevel))
			{
				// BSTODO: Temp
				//#ifndef NDEBUG
				ConsoleLog(Private::LogLevel::LogLevelToFormat(InLevel), 
					Private::LogLevel::LogLevelToColorCode(InLevel), 
					LogLevelString, 
					RawLogString);
				//#endif
				bWriteToFile = true;
			}

			if (bWriteToFile || Instance.bAlwaysWriteToFile)
			{
				const FString& OutputString = String::Format("{} [{}]: {}\n", LocalTimeString, LogLevelString, RawLogString);
				Instance.OutputStream << OutputString.GetData();
			}
#endif
			if (InLevel == ELogLevel::Fatal)
			{
#ifdef BS_DEBUG
				BS_DEBUG_BREAK();
#endif
				BS_TERMINATE();
			}
		}

	private:
		FLogger() {}

#ifdef BS_LOGGING_ON
		std::ofstream OutputStream;
		bool bAlwaysWriteToFile = false;
#endif
		ELogLevel LogLevelFlags = ELogLevel::All;
		bool bSessionStarted = false;

		FORCEINLINE static void StaticConstructor()
		{
#ifdef BS_LOGGING_ON
			BeginSession((ELogLevel::All), std::ios::trunc);
#endif
		}

		FORCEINLINE static void StaticDestructor()
		{
#ifdef BS_LOGGING_ON
			const FAllocationsInfo& AllocationsInfo = FAllocationLogger::GetAllocationsInfo();

			Log(ELogLevel::Info, "Allocated:   {}.B, {}.MB, {} Blocks",
				AllocationsInfo.AllocatedSize,
				AllocationsInfo.AllocatedSize >> 20ull,
				AllocationsInfo.AllocatedBlockCount);

			Log(ELogLevel::Info, "Deallocated: {}.B, {}.MB, {} Blocks",
				AllocationsInfo.DeallocatedSize,
				AllocationsInfo.DeallocatedSize >> 20ull,
				AllocationsInfo.DeallocatedBlockCount);

			FAllocationLogger::IsMemoryLeak()
				? Log(ELogLevel::Warning, "Memory leak detected")
				: Log(ELogLevel::Info, "Memory leak not detected");

			EndSession();
#endif
		}

		FORCEINLINE static void ConsoleLog(const char* const Format, 
			const char* const ColorCode, 
			const char* const LogLevel,
			const char* const Message)
		{
			printf(Format, ColorCode, LogLevel, Message);
		}

#ifdef BS_LOGGING_ON
		static void BeginSession(const ELogLevel Level, const int OutputStreamOpenMode, const bool bAlwaysWriteToFile = false);
		static void EndSession();
#endif
	};
}
