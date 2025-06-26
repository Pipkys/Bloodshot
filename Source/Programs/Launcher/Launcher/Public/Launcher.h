#pragma once

#include "Core.h"

#include "GUI.h"
#include "LauncherWindow.h"
#include "Settings.h"
#include "Translator.h"
#include "Windows/AddProjectWindow.h"
//#include "Windows/DebugWindow.h"
#include "Windows/MainWindow.h"
#include "Windows/NewProjectWindow.h"
#include "Windows/OpenProjectWindow.h"

#include <memory>

namespace Bloodshot::Launcher
{
	class FLauncher final
	{
	public:
		~FLauncher() {}

		FORCEINLINE static void Launch()
		{
			GetInstance().Run();
		}

	private:
		FLauncher() {}

		TList<ILauncherWindow*> InterfaceParts;
		bool bLaunched = false;

		NODISCARD static FLauncher& GetInstance();

		FORCEINLINE void Run()
		{
			if (bLaunched)
			{
				return;
			}

			(void)FGui::GetInstance();

			for (size_t FontSize = 4; FontSize <= 36; ++FontSize)
			{
				FGui::LoadFontFromFile(FSettings::FontPath.string().c_str(), (float)FontSize, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
			}

			using Shared::FTranslator;
			using Shared::ELanguage;
			const ELanguage CurrentLanguage = static_cast<Shared::ELanguage>(FSettings::GetLanguageIndex());
			FTranslator::SetLanguage(ELanguage::Russian);
			FTranslator::AddPhraseToCurrentDictionary("Projects", UTF8TEXT("Проекты"));
			FTranslator::AddPhraseToCurrentDictionary("Add", UTF8TEXT("Добавить"));
			FTranslator::AddPhraseToCurrentDictionary("New Project", UTF8TEXT("Новый Проект"));
			FTranslator::AddPhraseToCurrentDictionary("Settings", UTF8TEXT("Настройки"));
			FTranslator::AddPhraseToCurrentDictionary("Projects Path", UTF8TEXT("Путь Проектов"));
			FTranslator::AddPhraseToCurrentDictionary("Project Path", UTF8TEXT("Путь Проекта"));
			FTranslator::AddPhraseToCurrentDictionary("Create", UTF8TEXT("Создать"));
			FTranslator::AddPhraseToCurrentDictionary("Project Name", UTF8TEXT("Название Проекта"));
			FTranslator::AddPhraseToCurrentDictionary("Language", UTF8TEXT("Язык"));
			FTranslator::AddPhraseToCurrentDictionary("English", UTF8TEXT("Английский"));
			FTranslator::AddPhraseToCurrentDictionary("Russian", UTF8TEXT("Русский"));
			FTranslator::AddPhraseToCurrentDictionary("Add Project", UTF8TEXT("Добавить Проект"));
			FTranslator::SetLanguage(CurrentLanguage);

			FGui::LoadTextureFromFile("Logo512", FSettings::LogoPath.string().c_str());
			FGui::LoadTextureFromFile("SettingsIcon512", FSettings::SettingsIconPath.string().c_str());
			FGui::LoadTextureFromFile("BackIcon512", FSettings::BackIconPath.string().c_str());

			InterfaceParts.AddTail(&FMainWindow::GetInstance());
			//InterfaceParts.AddTail(&FDebugWindow::GetInstance());
			InterfaceParts.AddTail(&FAddProjectWindow::GetInstance());
			InterfaceParts.AddTail(&FNewProjectWindow::GetInstance());
			InterfaceParts.AddTail(&FOpenProjectWindow::GetInstance());

			bLaunched = true;

			while (FGui::IsRunning())
			{
				FGui::BeginRender();
				for (ILauncherWindow* const InterfacePart : InterfaceParts)
				{
					InterfacePart->Draw();
				}
				FGui::EndRender();
			}
		}
	};
}