#include "Project.h"

namespace Bloodshot::Shared
{
	namespace Private::Project
	{
		static void ReadLine(std::ifstream& InInputStream, FString& InLine, const std::function<void(FString&)>& InVisitor)
		{
			std::string Temp;
			std::getline(InInputStream, Temp);
			InLine = Temp.c_str();
			InVisitor(InLine);
		}

		static void ReadLine(std::fstream& InInputStream, FString& InLine, const std::function<void(FString&)>& InVisitor)
		{
			std::string Temp;
			std::getline(InInputStream, Temp);
			InLine = Temp.c_str();
			InVisitor(InLine);
		}

		static void ReadContent(std::ifstream& InInputStream, FString& InLine, const std::function<void(FString&)>& InVisitor)
		{
			std::string Temp;
			while (std::getline(InInputStream, Temp))
			{
				InLine = Temp.c_str();
				InVisitor(InLine);
			}
		}

		static void ReadContent(std::fstream& InInputStream, FString& InLine, const std::function<void(FString&)>& InVisitor)
		{
			std::string Temp;
			while (std::getline(InInputStream, Temp))
			{
				InLine = Temp.c_str();
				InVisitor(InLine);
			}
		}

		void LoadFromFileImpl(Bloodshot::Shared::FProject& OutResult, std::ifstream& InInputStream)
		{
			assert(InInputStream.is_open() && "FProject::LoadFromFile: Failed to open project file for reading");
			FString Buffer;
			FString* const Content = (FString*)&OutResult;
			for (size_t i = 0; i < sizeof(Bloodshot::Shared::FProject) / sizeof(FString); ++i)
			{
				FString& ToWrite = Content[i];
				ReadLine(InInputStream, Buffer, [&ToWrite](FString& InLine)
				{
					bool bNeedWrite = false;
					String::ParseTokens(InLine, " ", [&ToWrite, &bNeedWrite](FStringView InToken)
					{
						if (bNeedWrite)
						{
							ToWrite = InToken.SubStr(1, InToken.GetSize() - 2);
							return;
						}
						bNeedWrite = true;
					});
				});
			}
		}

		bool LoadFromDirectoryImpl(Bloodshot::Shared::FProject& OutResult, std::filesystem::directory_iterator InDirIt)
		{
			using namespace std::filesystem;
			directory_iterator EndIt{};
			for (; InDirIt != EndIt; ++InDirIt)
			{
				const std::filesystem::path& Dir = InDirIt->path();
				if (Dir.has_extension() && Dir.extension() == ".bsproject")
				{
					std::ifstream InputStream(Dir);
					LoadFromFileImpl(OutResult, InputStream);
					return true;
				}
			}
			return false;
		}
	}

	FProject::FProject(FStringView InDirectory)
	{
		*this = LoadFromDirectory(InDirectory);
	}

	FProject::FProject(const std::filesystem::path& InDirectory)
	{
		*this = LoadFromDirectory(InDirectory);
	}

	FProject FProject::LoadFromFile(FStringView InFilePath)
	{
		FProject Result;
		std::ifstream InputStream(InFilePath.GetData());
		Private::Project::LoadFromFileImpl(Result, InputStream);
		return Result;
	}

	FProject FProject::LoadFromDirectory(FStringView InDirectory)
	{
		FProject Result;
		Private::Project::LoadFromDirectoryImpl(Result, std::filesystem::directory_iterator(InDirectory.GetData()));
		return Result;
	}

	bool FProject::TryLoadFromDirectory(FProject& OutResult, FStringView InDirectory)
	{
		return Private::Project::LoadFromDirectoryImpl(OutResult, std::filesystem::directory_iterator(InDirectory.GetData()));
	}

	void FProject::SaveToFile(const std::filesystem::path& InFilePath)
	{
		std::ofstream OutputStream(InFilePath);
		BS_ASSERT(OutputStream.is_open(), "FProject::SaveToFile: Failed to open output file for saving");
		OutputStream << String::Format("Name: \"{}\"\nDirectory: \"{}\"\nContentDirectory: \"{}\"\nEditorStartupScene: \"{}\"\nGameDefaultScene: \"{}\"\n",
			Name,
			Directory,
			ContentDirectory,
			EditorStartupScene,
			GameDefaultScene).GetData();
	}

	FProject FProjectArchive::Get(const FString& InName, const std::filesystem::path& InDirectory)
	{
		Stream.open(OutputFileName.GetData(), std::ios_base::in);
		const FString& InDirStr = InDirectory.string().c_str();
		FProject Result;
		FString Buffer;
		Private::Project::ReadContent(Stream, Buffer, [this, &InName, &InDirStr, &Result](FString& InRecord)
		{
			if (InRecord.Contains(InName) && InRecord.Contains(InDirStr))
			{
				ParseRecord(Result, InRecord);
				return;
			}
		});
		Stream.close();
		return Result;
	}

	void FProjectArchive::Add(const FProject& InProject)
	{
		Stream.open(OutputFileName.GetData(), std::ios_base::out | std::ios_base::app);
		Stream << String::Format("={}?={}?={}?={}?={}\n",
			InProject.Name,
			InProject.Directory,
			InProject.ContentDirectory,
			InProject.EditorStartupScene,
			InProject.GameDefaultScene).GetData();
		Stream.close();
	}

	void FProjectArchive::Remove(const FProject& InProject)
	{
		Stream.open(OutputFileName.GetData(), std::ios_base::in);
		std::string Temp;
		FString Content;
		FString Record;
		while (std::getline(Stream, Temp))
		{
			Record = Temp.c_str();
			if (!Record.Contains(InProject.Name) || !Record.Contains(InProject.Directory))
			{
				Content += Record;
			}
		}
		Stream.close();
		Stream.open(OutputFileName.GetData(), std::ios_base::out | std::ios_base::trunc);
		Stream << Content.GetData();
		Stream.close();
	}

	bool FProjectArchive::Contains(const FProject& InProject)
	{
		Stream.open(OutputFileName.GetData(), std::ios_base::in);
		std::string Temp;
		FString Record;
		while (std::getline(Stream, Temp))
		{
			Record = Temp.c_str();
			if (Record.Contains(InProject.Name) && Record.Contains(InProject.Directory))
			{
				Stream.close();
				return true;
			}
		}
		Stream.close();
		return false;
	}

	void FProjectArchive::ForEach(const std::function<void(FProject&)>& InFunc)
	{
		Stream.open(OutputFileName.GetData(), std::ios_base::in);
		std::string Temp;
		FString Record;
		while (std::getline(Stream, Temp))
		{
			Record = Temp.c_str();
			FProject Project;
			ParseRecord(Project, Record);
			InFunc(Project);
		}
		Stream.close();
	}

	void FProjectArchive::ParseRecord(FProject& OutResult, FStringView InRecord)
	{
		FString* Content = (FString*)&OutResult;
		String::ParseTokens(InRecord, "?", [&Content](FStringView InToken)
		{
			FStringView CuttedValue = InToken.SubStr(1);
			*(Content++) = FString(CuttedValue.GetData(), CuttedValue.GetSize());
		});
	}
}
