#pragma once

#include "Core.h"

#include "OpenGL/ImGuiHeader.h"

#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>

namespace Bloodshot::Launcher
{
	class FSettings final
	{
		friend class FMainWindow;

	public:
		FORCEINLINE ~FSettings()
		{
			Save();
		}

		static inline const ImVec2 MinWindowSize = {1000, 500};

		static inline const std::filesystem::path FontPath = "../Resources/Common/Fonts/PT_Sans-Web-Regular.ttf";
		static inline const std::filesystem::path LogoPath = "../Resources/Launcher/Images/Logo512.png";
		static inline const std::filesystem::path SettingsIconPath = "../Resources/Launcher/Images/SettingsIcon512.png";
		static inline const std::filesystem::path BackIconPath = "../Resources/Launcher/Images/BackIcon512.png";

		NODISCARD static FSettings& GetInstance();

		NODISCARD FORCEINLINE static const FString& GetProjectsFolderPath()
		{
			return GetInstance().ProjectsFolderPath;
		}

		NODISCARD FORCEINLINE static size_t GetProjectsFolderPathSize()
		{
			return ProjectsFolderPathSize;
		}

		NODISCARD FORCEINLINE static int GetLanguageIndex()
		{
			return GetInstance().LanguageIndex;
		}

	private:
		FORCEINLINE FSettings()
		{
			FFilesystem::CreateDirectories("Config");
			Load();
		}

		const char* Filename = "Config/Launcher Config.ini";
		static inline constexpr const size_t ProjectsFolderPathSize = 1024;
		FString ProjectsFolderPath;
		int LanguageIndex;

		void Save()
		{
			std::ofstream OutputStream(Filename, std::ios::out | std::ios::trunc);
			OutputStream << "ProjectsFolderPath: \"" << ProjectsFolderPath.GetData() << "\"\n";
			OutputStream << "LanguageIndex: \"" << LanguageIndex << "\"";
			OutputStream.close();
		}

		FORCEINLINE void Load()
		{
			const bool bSettingsFound = std::filesystem::exists(Filename);

			if (!bSettingsFound)
			{
				ProjectsFolderPath = FString(std::filesystem::current_path().string().c_str()).Append("\\Projects");
				LanguageIndex = 0;
			}
			else
			{
				std::ifstream InputStream(Filename);
				std::string Temp;
				FString Line;
				bool bWrite = false;
				std::getline(InputStream, Temp);
				Line = Temp.c_str();
				String::ParseTokens(Line, " ", [this, &bWrite](FStringView InToken)
				{
					if (bWrite)
					{
						ProjectsFolderPath = InToken.SubStr(1, InToken.GetSize() - 2);
					}
					bWrite = true;
				});
				bWrite = false;
				std::getline(InputStream, Temp);
				Line = Temp.c_str();
				String::ParseTokens(Line, " ", [this, &bWrite](FStringView InToken)
				{
					if (bWrite)
					{
						LanguageIndex = std::stoi(InToken.SubStr(1, InToken.GetSize() - 2).GetData());
					}
					bWrite = true;
				});
				InputStream.close();
			}
		}
	};
}
