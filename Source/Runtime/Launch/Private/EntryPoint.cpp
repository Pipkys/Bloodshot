#include "Logging/Logger.h"
#include "Profiling/Profiler.h"

namespace Bloodshot::Private::Launch
{
	struct FDebugEntry final
	{
		FORCEINLINE FDebugEntry()
		{
			FProfiler::StaticConstructor();
			FLogger::StaticConstructor();
		}

		FORCEINLINE ~FDebugEntry()
		{
			FLogger::StaticDestructor();
			FProfiler::StaticDestructor();
		}
	} GDebugEntry;
}

#include "EngineLoop.h"

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;

	Shared::FCmdParser CmdParser(Argc, Argv);
	Private::Launch::FEngineLoop EngineLoop;

	EngineLoop.PreInit(CmdParser);
	EngineLoop.Init();
	EngineLoop.Run();
	EngineLoop.Exit();
	EngineLoop.Dispose();
}
