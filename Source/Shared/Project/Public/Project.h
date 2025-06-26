#pragma once

#include "Core.h"

#include <filesystem>
#include <fstream>
#include <functional>

namespace Bloodshot::Shared
{
	struct FProject;

	namespace Private::Project
	{
		void LoadFromFileImpl(Bloodshot::Shared::FProject& OutResult, std::ifstream& InInputStream);
		bool LoadFromDirectoryImpl(Bloodshot::Shared::FProject& OutResult, std::filesystem::directory_iterator InDirIt);
	}

	struct FProject final
	{
		FProject() {}
		FProject(FStringView InDirectory);
		FProject(const std::filesystem::path& InDirectory);

		FString Name;
		FString Directory;
		FString ContentDirectory;
		FString EditorStartupScene;
		FString GameDefaultScene;

		static FProject LoadFromFile(FStringView InFilePath);

		template<typename T>
			requires std::_Is_any_path<T>
		static FProject LoadFromFile(const T& InFilePath)
		{
			FProject Result;
			std::ifstream InputStream(InFilePath);
			Private::Project::LoadFromFileImpl(Result, InputStream);
			return Result;
		}

		static FProject LoadFromDirectory(FStringView InDirectory);

		template<typename T>
			requires std::_Is_any_path<T>
		static FProject LoadFromDirectory(const T& InDirectory)
		{
			FProject Result;
			Private::Project::LoadFromDirectoryImpl(Result, std::filesystem::directory_iterator(InDirectory));
			return Result;
		}

		static bool TryLoadFromDirectory(FProject& OutResult, FStringView InDirectory);

		template<typename T>
			requires std::_Is_any_path<T>
		static bool TryLoadFromDirectory(FProject& OutResult, const T& InDirectory)
		{
			return Private::Project::LoadFromDirectoryImpl(OutResult, std::filesystem::directory_iterator(InDirectory));
		}

		void SaveToFile(const std::filesystem::path& InFilePath);
	};

	class FProjectArchive final
	{
	public:
		FProjectArchive(FStringView InOutputFileName)
			: OutputFileName(InOutputFileName)
		{
		}

		FProject Get(const FString& Name, const std::filesystem::path& Directory);
		void Add(const FProject& InProject);
		void Remove(const FProject& InProject);
		bool Contains(const FProject& InProject);
		void ForEach(const std::function<void(FProject&)>& Func);

	private:
		std::fstream Stream;
		FString OutputFileName;

		void ParseRecord(FProject& OutResult, FStringView InRecord);
	};
}
