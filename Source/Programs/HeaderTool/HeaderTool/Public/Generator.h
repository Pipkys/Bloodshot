#pragma once

#include "Core.h"

#include "TypeInfo.h"

#include <filesystem>
#include <format>
#include <sstream>

namespace Bloodshot::HeaderTool
{
	struct FHeaderFileInfo
	{
		TArray<FClassInfo> ClassInfos;
		FString Path;
		FString ModuleName;
	};

	struct FGeneratorOutput
	{
		FString SourceOutput;
		FString HeaderOutput;
	};

	class FGenerator final
	{
	public:
		FGeneratorOutput Generate(const FHeaderFileInfo& HeaderInfo,
			const TArray<FHeaderFileInfo>& HeadersInfo);

	private:
		FString SourceOutput;
		FString HeaderOutput;

		FString* CurrentOutput;
		size_t PushedScopes = 0;

		void WriteLine(FStringView String);

		template<typename... ArgTypes>
		void WriteLine(FString InFormat, ArgTypes&&... InArgs)
		{
			for (size_t i = 0; i < PushedScopes; ++i)
			{
				*CurrentOutput += "\t";
			}

			*CurrentOutput += String::Format(InFormat, std::forward<ArgTypes>(InArgs)...) + "\n";
		}

		void PushScope();
		void PopScope();
		void Clear();

		void WriteClassConstructionImpl(const FHeaderFileInfo& HeaderInfo, const TArray<FHeaderFileInfo>& HeadersInfo);
		void WriteSerializeionImpl();
		void WriteReplicationImpl();
	};
}
