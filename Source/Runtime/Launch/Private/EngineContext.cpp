#include "EngineContext.h"
#include "RHI.h"
#include "World.h"

namespace Bloodshot::Private::Launch
{
	void IEngineContext::PreInit()
	{
#if defined(BS_DEBUG) && defined(BS_WINDOWS)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		BS_PROFILE_RANGE("Core Init");
		BS_LOG(Debug, "Initializing the Core...");

		Object::FObjectCore::StaticConstructor();
		FRHI::StaticConstructor();

		FEngineState::GetInstance().bRunning = true;
	}

	void IEngineContext::Dispose()
	{
		BS_PROFILE_RANGE("Core Dispose");
		BS_LOG(Debug, "Destroying the Core...");

		FRHI::StaticDestructor();
		Object::FObjectCore::StaticDestructor();

		FEngineState::GetInstance().bRunning = false;
	}

	void IEngineContext::BeginPlay()
	{
#ifdef BS_EXCEPTION_SAFETY_ON
		try
		{
#endif
			BS_PROFILE_RANGE("BeginPlay");
			BS_LOG(Debug, "BeginPlay");

			FWorld::BeginPlay();
#ifdef BS_EXCEPTION_SAFETY_ON
		}
		catch (const std::exception& Exception)
		{
			BS_LOG(Error, "Exception occurred in BeginPlay: {}", Exception.what());
		}
		catch (...)
		{
			BS_LOG(Error, "Undefined exception occurred in BeginPlay");
		}
#endif
	}

	void IEngineContext::EndPlay()
	{
#ifdef BS_EXCEPTION_SAFETY_ON
		try
		{
#endif
			BS_PROFILE_RANGE("EndPlay");
			BS_LOG(Debug, "EndPlay");

			FWorld::EndPlay();
#ifdef BS_EXCEPTION_SAFETY_ON
		}
		catch (const std::exception& Exception)
		{
			BS_LOG(Error, "Exception occurred in EndPlay: {}", Exception.what());
		}
		catch (...)
		{
			BS_LOG(Error, "Undefined exception occurred in EndPlay");
		}
#endif
	}
}
