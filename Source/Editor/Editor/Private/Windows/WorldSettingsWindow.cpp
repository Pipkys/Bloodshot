#include "Windows/WorldSettingsWindow.h"
#include "Editor.h"
#include "ImGuiHeader.h"
#include "Translator.h"

namespace Bloodshot
{
	void FWorldSettingsWindow::Input()
	{
	}

	void FWorldSettingsWindow::Tick()
	{
	}

	void FWorldSettingsWindow::Render()
	{
		if (ImGui::Ext::Begin(TRANSLATE("World Settings"), bFocused))
		{

		}
		ImGui::End();
	}
}
