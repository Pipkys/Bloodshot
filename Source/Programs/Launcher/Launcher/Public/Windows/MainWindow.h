#pragma once

#include "Core.h"

#include "LauncherWindow.h"
#include "Windows/ChangelogTab.h"
#include "Windows/ProjectsTab.h"

namespace Bloodshot::Launcher
{
	class FMainWindow final : public ILauncherWindow
	{
	public:
		NODISCARD static FMainWindow& GetInstance();

	private:
		FORCEINLINE FMainWindow()
		{
			Setup();
		}

		FString CurrentPage = "Main";
		FString CurrentTab = "Projects";

		virtual void Draw() override;

		void SetupMainPage();
		void SetupSettingsPage();
		void Setup();
	};
}
