#pragma once

#include "Core.h"

#include "CmdParser.h"
#include "EngineContext.h"

namespace Bloodshot::Private::Launch
{
	class FEngineGameContext : public IEngineContext
	{
	public:
		FEngineGameContext() = default;
		virtual ~FEngineGameContext() override {}

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) override;
		virtual void Init() override;
		virtual void Run() override;
		virtual void Exit() override;
	};
}
