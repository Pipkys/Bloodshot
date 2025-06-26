#pragma once

#include "Core.h"

#include "LauncherWindow.h"

#include <filesystem>

namespace Bloodshot::Launcher
{
	struct FProject;

	class FNewProjectWindow final : public ILauncherWindow
	{
	public:
		NODISCARD static FNewProjectWindow& GetInstance();

		FORCEINLINE static void Open()
		{
			GetInstance().ResetInputTextBoxes();
			GetInstance().bOpened = true;
		}

		FORCEINLINE static void Close()
		{
			GetInstance().bOpened = false;
		}

	private:
		bool bOpened = false;

		FORCEINLINE FNewProjectWindow()
		{
			Setup();
		}

		virtual void Draw() override;

		void Setup();

		bool NewProject(const FString& Name, const std::filesystem::path& TargetFolderPath);

		void ResetInputTextBoxes();
	};
}
