#pragma once

#include "Core.h"

#include "GUI.h"
#include "LauncherWindow.h"
#include "Project.h"

namespace Bloodshot::Launcher
{
	class FProjectsTab final : public ILauncherWindow
	{
		friend class FMainWindow;
		friend class FAddProjectWindow;
		friend class FNewProjectWindow;
		friend class FOpenProjectWindow;

	public:
		FORCEINLINE ~FProjectsTab() {}

		NODISCARD static FProjectsTab& GetInstance();

	private:
		FORCEINLINE FProjectsTab()
			: ProjectArchive("Config/Launcher Projects.ini")
		{
			FFilesystem::CreateDirectories("Config");
		}

		Shared::FProjectArchive ProjectArchive;
		TList<Shared::FProject> Projects;

		bool bNeedReloadProjects = true;
		bool bAddProjectWindowOpened = false;
		bool bNewProjectWindowOpened = false;
		bool bOpenProjectWindowOpened = false;

		virtual void Draw() override;

		void UpdateProjects(const std::filesystem::path& TargetFolderPath);
	};
}
