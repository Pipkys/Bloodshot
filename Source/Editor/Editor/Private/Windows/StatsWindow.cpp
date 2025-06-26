#include "Windows/StatsWindow.h"
#include "EditorConfig.h"
#include "Misc/EngineState.h"
#include "Misc/EngineTime.h"
#include "RHI.h"
#include "Translator.h"

namespace Bloodshot
{
	void FStatsWindow::Input()
	{
	}

	void FStatsWindow::Tick()
	{
	}

	void FStatsWindow::Render()
	{
		if (ImGui::Ext::Begin(TRANSLATE("Stats"), bFocused))
		{
			ImGui::Text("Frame Time: %f", FRHI::GetFrameTime());
			ImGui::Text("Engine State: %s", FEngineState::IsSimulating() ? "Simulating" : "Editing");
			ImGui::Text("Engine Time: %f", FEngineTime::GetGlobalTime());
		}
		ImGui::End();
	}
}
