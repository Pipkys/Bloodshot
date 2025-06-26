#include "EngineEditorContext.h"
#include "AssetManager.h"
#include "CmdParser.h"
#include "Editor.h"
#include "NetworkingSystem.h"
#include "RHI.h"
#include "Translator.h"
#include "World.h"
#include "WorldRenderer.h"

namespace Bloodshot::Private::Launch
{
	void FEngineEditorContext::LoadConfig(Shared::FCmdParser& CmdParser)
	{
		//CmdParser.AddOption("Language");
		//CmdParser.AddOption("Project");
		//CmdParser.Parse();
		//if (!CmdParser.HasAllOptions())
		//{
		//	CmdParser.BuildErrorMessage();
		//	BS_LOG(Fatal, "{}", CmdParser.GetErrorMessage());
		//}
		LanguageIndex = 0; //std::stoi(CmdParser.GetOptionValue(0).GetData());
		//ProjectPath = CmdParser.GetOptionValue(1);
	}

	void FEngineEditorContext::Init()
	{
		// BSTODO: Store into .bsproject file
		//GameDLLObserver.SetTimestampPath(CurrentProject.Directory.string() + "/Binaries/Timestamp");

		Shared::FTranslator::SetLanguage(static_cast<Shared::ELanguage>(LanguageIndex));
		FAssetManager::StaticConstructor();
		FWorldRenderer::StaticConstructor();
		FEditor::StaticConstructor();
		FEditor::SetOnStartSimulateEvent([this]() { BeginPlay(); });
		FEditor::SetOnEndSimulateEvent([this]() { EndPlay(); });
	}

	void FEngineEditorContext::Run()
	{
		while (!FRHI::IsExitRequested() && !FApplication::IsExitRequested())
		{
#ifdef BS_EXCEPTION_SAFETY_ON
			try
			{
#endif
				BS_PROFILE_RANGE("Main Loop");
				const float DeltaTime = FRHI::GetFrameTime();

				FEngineTime::Tick();
				FRHI::BeginFrame();

#ifdef BS_NETWORKING_ON
				if (FEngineState::IsSimulating())
				{
					FNetworkingSystem::Execute(DeltaTime);
				}
#endif
				if (FEngineState::IsSimulating())
				{
					FWorld::Tick(DeltaTime);
				}

				FEditor::Tick();
				FEditor::RenderGizmos();
				FEditor::RenderGui();

				FRHI::SwapBuffers();
				FRHI::PollEvents();
				FRHI::EndFrame();

#ifdef BS_EXCEPTION_SAFETY_ON
			}
			catch (const std::exception& Exception)
			{
				BS_LOG(Error, "Exception occurred: {}", Exception.what());
			}
			catch (...)
			{
				BS_LOG(Error, "Undefined exception occurred");
			}
#endif
			//GameDLLObserver.Observe();
		}
	}

	void FEngineEditorContext::Exit()
	{
		FEditor::StaticDestructor();
		FWorldRenderer::StaticDestructor();
		FAssetManager::StaticDestructor();
	}
}
