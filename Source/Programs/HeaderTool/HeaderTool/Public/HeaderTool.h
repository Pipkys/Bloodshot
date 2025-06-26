#pragma once

#include "Core.h"

#include "Generator.h"
#include "Parser.h"
#include "Tokenizer.h"

#include <filesystem>
#include <fstream>

namespace Bloodshot::HeaderTool
{
	class FHeaderTool final
	{
	public:
		FORCEINLINE void Setup(FStringView InSourcePath, FStringView InOutputPath)
		{
			SourcePath = InSourcePath;
			OutputPath = InOutputPath;
		}

		FORCEINLINE void Setup(FStringView InSourcePath, FStringView InOutputPath, FStringView InModuleName)
		{
			Setup(InSourcePath, InOutputPath);
			CurrentModuleName = InModuleName;
		}

		FORCEINLINE void Launch()
		{
			if (FFilesystem::HasExtension(SourcePath) && FFilesystem::GetExtension(SourcePath) == ".h")
			{
				ParseHeader(SourcePath);
			}
			else
			{
				ParseHeaders(SourcePath);
			}

			for (const FHeaderFileInfo& HeaderInfo : HeadersData)
			{
				if (HeaderInfo.ModuleName == "Core" && FFilesystem::GetFilename(HeaderInfo.Path) == "Object.h")
				{
					continue;
				}
				ReflectHeader(HeaderInfo);
			}
		}

	private:
		FString SourcePath;
		FString OutputPath;
		FString CurrentModuleName;

		std::ofstream SourceOutputStream;
		std::ofstream HeaderOutputStream;

		FTokenizer Tokenizer;
		FParser Parser;
		FGenerator Generator;

		TArray<FHeaderFileInfo> HeadersData;

		FORCEINLINE void ParseHeader(FStringView InPath)
		{
			std::ifstream InputStream(InPath.GetData());
			BS_LOG_IF(!InputStream.is_open(),
				Fatal,
				"FHeaderTool::CollectHeaderData: Failed to open input header file: {}",
				InPath);

			std::stringstream StringStream;
			StringStream << InputStream.rdbuf();
			InputStream.close();

			const FString& SourceCode = StringStream.str().c_str();
			const TArray<FToken>& Tokens = Tokenizer.Tokenize(SourceCode);

			FHeaderFileInfo HeaderInfo;
			HeaderInfo.ClassInfos = Parser.Parse(Tokens);

			if (!HeaderInfo.ClassInfos.GetSize())
			{
				return;
			}

			HeaderInfo.Path = InPath;
			HeaderInfo.ModuleName = CurrentModuleName;
			HeadersData.EmplaceBack(std::move(HeaderInfo));
		}

		FORCEINLINE void ParseHeaders(FStringView InFolderPath)
		{
			for (const std::filesystem::directory_entry& DirectoryEntry : std::filesystem::recursive_directory_iterator(InFolderPath.GetData()))
			{
				FString Filepath = DirectoryEntry.path().string().c_str();

				if (FFilesystem::GetFilename(Filepath) == "CMakeLists.txt")
				{
					FStringView ModulePath = FFilesystem::GetParentPath(Filepath);
					CurrentModuleName = FFilesystem::GetBaseFilename(ModulePath);
					continue;
				}

				if (!FFilesystem::HasExtension(Filepath) || FFilesystem::GetExtension(Filepath) != ".h")
				{
					continue;
				}

				ParseHeader(Filepath);
			}
		}

		FORCEINLINE void ReflectHeader(const FHeaderFileInfo& InHeaderInfo)
		{
			const FString& HeaderPath = InHeaderInfo.Path;
			const FGeneratorOutput& GeneratorOutput = Generator.Generate(InHeaderInfo, HeadersData);

			const FString& FullOutputPathString = OutputPath + "/" + InHeaderInfo.ModuleName;
			const FString& CurrentHeaderFileNameString = FFilesystem::GetBaseFilename(HeaderPath);

			FFilesystem::CreateDirectories(FullOutputPathString);

			SourceOutputStream.close();
			SourceOutputStream.open((FullOutputPathString + "/" + CurrentHeaderFileNameString + ".gen.cpp").GetData());
			SourceOutputStream << GeneratorOutput.SourceOutput.GetData();

			HeaderOutputStream.close();
			HeaderOutputStream.open((FullOutputPathString + "/" + CurrentHeaderFileNameString + ".generated.h").GetData());
			if (GeneratorOutput.HeaderOutput.GetSize())
			{
				HeaderOutputStream << GeneratorOutput.HeaderOutput.GetData();
			}
		}
	};
}
