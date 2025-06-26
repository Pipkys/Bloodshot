#include "Logging/Logger.h"
#include "Misc/AssertionMacros.h"

#include <filesystem>

namespace Bloodshot
{
	FLogger& FLogger::GetInstance()
	{
		static FLogger Instance;
		return Instance;
	}

#ifdef BS_LOGGING_ON
	void FLogger::BeginSession(const ELogLevel InLogLevelFlags,
		const int InOutputStreamOpenMode,
		const bool bInAlwaysWriteToFile)
	{
		FLogger& Instance = GetInstance();
		bool& bStarted = Instance.bSessionStarted;
		BS_CHECK(!bStarted);

		Instance.LogLevelFlags = InLogLevelFlags;
		Instance.bAlwaysWriteToFile = bInAlwaysWriteToFile;

		if (!std::filesystem::exists("Logs"))
		{
			std::filesystem::create_directory("Logs");
		}

		std::ofstream& OutputStream = Instance.OutputStream;
		OutputStream.open("Logs/Logger.txt", InOutputStreamOpenMode);

		BS_CHECK(OutputStream.is_open());
		Log(ELogLevel::Info, "Logging Session started");

		bStarted = true;
	}

	void FLogger::EndSession()
	{
		FLogger& Instance = GetInstance();
		BS_CHECK(Instance.bSessionStarted);
		Log(ELogLevel::Info, "Logging Session ended");
		Instance.OutputStream.close();
	}
#endif
}
