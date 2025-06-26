#pragma once

#include "Core.h"

#include "Token.h"
#include "TypeInfo.h"

namespace Bloodshot::HeaderTool
{
	class FParser final
	{
	public:
		TArray<FClassInfo> Parse(const TArray<FToken>& Tokens);

	private:
		struct FNamespaceInfo final
		{
			size_t Index;
			FString Name;
		};

		TArray<FClassInfo> ClassInfos;
		TArray<FToken> Tokens;
		size_t TokenIndex = 0;
		FClassInfo TempClassInfo;
		TArray<FNamespaceInfo> NamespaceStack;

		const FToken& Get();
		const FString& GetValue();
		ETokenType GetType();

		const FToken& Consume();
		const FString& ConsumeValue();
		ETokenType ConsumeType();

		void Require(FStringView RequiredToken);
		void Skip(FStringView TokenToSkip);

		void ParseType(FString& OutTypeStr, TArray<FToken>& OutTokens);

		void ParseClassMacroArgs();
		void ParseFunctionMacroArgs(FFunctionInfo& OutFunctionInfo);
		void ParsePropertyMacroArgs(FPropertyInfo& OutPropertyInfo);

		void ParseClassDeclaration();
		void ParseClassDefinition();
		void ParseClass();

		void ParseFunctionParameters(FFunctionInfo& OutFunctionInfo);
		void ParseFunction();
		void ParseProperty();
	};
}
