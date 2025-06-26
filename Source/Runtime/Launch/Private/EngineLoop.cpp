#include "EngineLoop.h"
#include "Editor/EngineEditorContext.h"
#include "Game/EngineGameContext.h"

namespace Bloodshot::Private::Launch
{
	void FEngineLoop::PreInit(Shared::FCmdParser& CmdParser)
	{
#ifdef BS_WITH_EDITOR
		EngineContext = new FEngineEditorContext;
#else
		EngineContext = new FEngineGameContext;
#endif
		EngineContext->LoadConfig(CmdParser);
		EngineContext->PreInit();
	}

	void FEngineLoop::Init()
	{
		EngineContext->Init();
	}

	void FEngineLoop::Run()
	{
		EngineContext->Run();
	}

	void FEngineLoop::Exit()
	{
		EngineContext->Exit();
	}

	void FEngineLoop::Dispose()
	{
		EngineContext->Dispose();
		delete EngineContext;
	}
}
