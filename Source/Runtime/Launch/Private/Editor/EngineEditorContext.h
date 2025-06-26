#pragma once

#include "Core.h"

#include "EngineContext.h"

namespace Bloodshot::Private::Launch
{
	class FEngineEditorContext : public IEngineContext
	{
	public:
		FEngineEditorContext() = default;
		virtual ~FEngineEditorContext() override {}

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) override;
		virtual void Init() override;
		virtual void Run() override;
		virtual void Exit() override;

	private:
		int LanguageIndex;
		FString ProjectPath;
	};
}
