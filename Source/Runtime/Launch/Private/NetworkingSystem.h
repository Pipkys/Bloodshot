#pragma once

#ifdef BS_NETWORKING_ON

#include "Core.h"

namespace Bloodshot::Private::Launch
{
	class FNetworkingSystem final
	{
		friend class FEngineEditorContext;
		friend class FEngineGameContext;

		static void Execute(float DeltaTime);
	};
}

#endif