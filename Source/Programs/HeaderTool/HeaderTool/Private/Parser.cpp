#include "Keywords.h"
#include "Parser.h"

#include <format>

namespace Bloodshot::HeaderTool
{
	TArray<FClassInfo> FParser::Parse(const TArray<FToken>& Tokens)
	{
		this->Tokens = Tokens;
		ClassInfos.Clear();
		TokenIndex = 0;
		TempClassInfo = FClassInfo();
		NamespaceStack.Clear();
		size_t BraceCount = 0;

		while (TokenIndex < Tokens.GetSize())
		{
			if (GetValue() == "namespace")
			{
				Consume();

				TArray<FNamespaceInfo> TempNamespaceStack;
				bool bInstruction = false;

				while (GetValue() != "{")
				{
					if (GetValue() == ";")
					{
						bInstruction = true;
						break;
					}
					else if (GetValue() != "::")
					{
						TempNamespaceStack.EmplaceBack(FNamespaceInfo(BraceCount, GetValue()));
					}

					Consume();
				}

				if (!bInstruction)
				{
					NamespaceStack.Append(TempNamespaceStack);
					++BraceCount;
				}
			}
			else if (GetValue() == "{")
			{
				++BraceCount;
			}
			else if (GetValue() == "}")
			{
				while (NamespaceStack.GetSize())
				{
					const FNamespaceInfo& LastNamespaceInfo = NamespaceStack.Last();
					if (LastNamespaceInfo.Index == BraceCount - 1)
					{
						NamespaceStack.PopBack();
						continue;
					}
					break;
				}

				--BraceCount;
			}

			if (GetValue() == "BSCLASS" && Tokens[TokenIndex - 1].Value != "define")
			{
				for (const FNamespaceInfo& NamespaceInfo : NamespaceStack)
				{
					TempClassInfo.Namespace += FString("::").Append(NamespaceInfo.Name);
				}

				Consume();
				ParseClass();
				continue;
			}

			Consume();
		}

		return ClassInfos;
	}

	const FToken& FParser::Get()
	{
		return Tokens[TokenIndex];
	}

	const FString& FParser::GetValue()
	{
		return Tokens[TokenIndex].Value;
	}

	ETokenType FParser::GetType()
	{
		return Tokens[TokenIndex].Type;
	}

	const FToken& FParser::Consume()
	{
		return Tokens[TokenIndex++];
	}

	const FString& FParser::ConsumeValue()
	{
		return Tokens[TokenIndex++].Value;
	}

	ETokenType FParser::ConsumeType()
	{
		return Tokens[TokenIndex++].Type;
	}

	void FParser::Require(FStringView RequiredToken)
	{
		const FString& RealToken = ConsumeValue();
		BS_LOG_IF(RealToken != RequiredToken, Fatal, "Required: {}, but found: {}", RequiredToken, RealToken)
	}

	void FParser::Skip(FStringView TokenToSkip)
	{
		while (GetValue() == TokenToSkip)
		{
			Consume();
		}
	}

	void FParser::ParseType(FString& OutTypeStr, TArray<FToken>& OutTokens)
	{
		size_t TemplateBraceCount = 0;

		bool bTemplateSpecializationStarted = false;
		bool bTemplateSpecializationEnded = false;
		bool bTypeCompleted = false;

		TArray<FString> ParsedTokens;

		while (OutTokens.GetSize() > 0)
		{
			const FToken& Token = OutTokens.Last();
			const FString& TokenValue = Token.Value;

			auto ExtractToken = [&OutTokens, &ParsedTokens]() -> void
			{
				ParsedTokens.PushBack(OutTokens.Last().Value);
				OutTokens.PopBack();
			};

			if (TokenValue == ">")
			{
				++TemplateBraceCount;
				bTemplateSpecializationStarted = true;
			}
			else if (TokenValue == "<")
			{
				--TemplateBraceCount;
				if (!TemplateBraceCount) bTemplateSpecializationEnded = true;
			}
			else if (TokenValue == "::")
			{
				ExtractToken();
			}
			else if (!bTemplateSpecializationStarted || bTemplateSpecializationStarted && bTemplateSpecializationEnded)
			{
				if (Token.Type == ETokenType::Identifier)
				{
					if (!bTypeCompleted)
					{
						bTypeCompleted = true;
					}
					else
					{
						OutTokens.PopBack();
						continue;
					}
				}
			}

			ExtractToken();
		}

		ParsedTokens.Reverse();

		for (const FString& Token : ParsedTokens)
		{
			OutTypeStr += Token;

			if (Token == "const" || Token == "volatile" || Token == "constexpr" || Token == "consteval")
			{
				OutTypeStr += " ";
			}
		}
	}

	void FParser::ParseClassMacroArgs()
	{
		Require("(");
		while (ConsumeValue() != ")")
		{
		}
	}

	void FParser::ParseFunctionMacroArgs(FFunctionInfo& OutFunctionInfo)
	{
		Require("(");
		while (ConsumeValue() != ")") {}
	}

	void FParser::ParsePropertyMacroArgs(FPropertyInfo& OutPropertyInfo)
	{
		Require("(");

		while (GetValue() != ")")
		{
			const FString& Value = ConsumeValue();

			//if (Value == "Serialized")
			//{
			//	OutPropertyInfo.bSerialized = true;
			//}
			//else if (Value == "Replicated")
			//{
			//	OutPropertyInfo.bReplicated = true;
			//}
		}

		Consume();
	}

	void FParser::ParseClassDeclaration()
	{
		TempClassInfo.Name = ConsumeValue();

		while (GetValue() != "{")
		{
			ETokenType Type = GetType();
			const FString& Value = ConsumeValue();

			if (Value == "final")
			{
				TempClassInfo.bFinal = true;
			}
			else if (Value == "abstract")
			{
				TempClassInfo.bAbstract = true;
			}
			else if (Value == ":")
			{
				TempClassInfo.bDerived = true;
			}
			else if (Type == ETokenType::Identifier)
			{
				TempClassInfo.BaseClassNames.PushBack(Value);
			}
		}

		Consume();
	}

	void FParser::ParseClassDefinition()
	{
		size_t BraceCount = 1;

		while (BraceCount > 0)
		{
			const FString& Value = GetValue();

			if (Value == "{")
			{
				++BraceCount;
			}
			else if (Value == "}")
			{
				--BraceCount;
			}
			else if (Value == "BSFUNCTION")
			{
				Consume();
				ParseFunction();
				continue;
			}
			else if (Value == "BSPROPERTY")
			{
				Consume();
				ParseProperty();
			}

			Consume();
		}
	}

	void FParser::ParseClass()
	{
		ParseClassMacroArgs();
		Skip(";");
		Require("class");
		ParseClassDeclaration();
		ParseClassDefinition();

		ClassInfos.EmplaceBack(std::move(TempClassInfo));
	}

	void FParser::ParseFunctionParameters(FFunctionInfo& OutFunctionInfo)
	{
		size_t BraceCount = 1;

		TArray<FToken> ParametersTokens;

		while (BraceCount > 0)
		{
			const FToken& Token = Consume();
			const FString& Value = Token.Value;

			if (Value == "(")
			{
				++BraceCount;
				ParametersTokens.PushBack(Token);
			}
			else if (Value == ")")
			{
				--BraceCount;
				if (BraceCount)
				{
					ParametersTokens.PushBack(Token);
				}
			}
			else
			{
				ParametersTokens.PushBack(Token);
			}
		}

		if (!ParametersTokens.GetSize())
		{
			return;
		}

		TArray<FToken> ParameterTokens;
		auto ExtractVariableWithoutInitializer = [&OutFunctionInfo, &ParameterTokens, this]()
		{
			ParameterTokens.Reverse();
			TArray<FToken>::FIterator EndIt = ParameterTokens.CreateIterator();
			EndIt.SetToEnd();
			TArray<FToken>::FIterator EraseStartIt = EndIt;
			for (TArray<FToken>::FIterator It = ParameterTokens.CreateIterator(); It != EndIt; ++It)
			{
				const FString& TokenValue = It->Value;
				if (TokenValue == "=" || TokenValue == "{" || TokenValue == "(")
				{
					EraseStartIt.SetIndex(It);
				}
			}
			const size_t ToRemoveCount = EndIt.GetIndex() - EraseStartIt.GetIndex();
			ParameterTokens.RemoveAt(EraseStartIt.GetIndex(), ToRemoveCount);

			FParameterInfo ParameterInfo;
			ParameterInfo.Name = ParameterTokens.Last().Value;
			ParameterTokens.PopBack();

			ParseType(ParameterInfo.Type, ParameterTokens);

			OutFunctionInfo.Parameters.EmplaceBack(std::move(ParameterInfo));
			ParameterTokens.Clear();
		};

		while (ParametersTokens.GetSize() > 0)
		{
			const FToken& Token = ParametersTokens.Last();
			const FString& TokenValue = Token.Value;

			if (TokenValue == ",")
			{
				ExtractVariableWithoutInitializer();
			}
			else
			{
				ParameterTokens.EmplaceBack(Token);
			}

			ParametersTokens.PopBack();
		}

		if (ParameterTokens.GetSize())
		{
			ExtractVariableWithoutInitializer();
		}
	}

	void FParser::ParseFunction()
	{
		FFunctionInfo FunctionInfo;

		ParseFunctionMacroArgs(FunctionInfo);
		Skip(";");

		TArray<FToken> TempTokens;

		// BSTODO: Also function maybe without body, so need to check for ; in the end

		// BSTODO: Parse return type correctly, it maybe c-style function like void(*)(...) or std::function<void()>, etc...
		// so it will be broken
		while (GetValue() != "(")
		{
			TempTokens.PushBack(Consume());
		}

		FunctionInfo.Name = TempTokens.Last().Value;
		TempTokens.PopBack();

		// BSTODO: Check after ) and until { for const and noexcept words
		if (TempTokens.FindByPredicate([](const FToken& Token)
		{
			return Token.Value == "static";
		}) != IReservedValues::NoneIndex)
		{
			FunctionInfo.bStatic = true;
		}

		ParseType(FunctionInfo.ReturnType, TempTokens);
		Consume();
		ParseFunctionParameters(FunctionInfo);

		TempClassInfo.Functions.EmplaceBack(std::move(FunctionInfo));
	}

	void FParser::ParseProperty()
	{
		FPropertyInfo PropertyInfo;

		ParsePropertyMacroArgs(PropertyInfo);
		Skip(";");

		TArray<FToken> TempTokens;

		while (GetValue() != ";" && GetValue() != "{" && GetValue() != "=")
		{
			TempTokens.PushBack(Consume());
		}

		if (TempTokens.FindByPredicate([](const FToken& Token)
		{
			return Token.Value == "static";
		}) != IReservedValues::NoneIndex)
		{
			PropertyInfo.bStatic = true;
		}

		PropertyInfo.Name = TempTokens.Last().Value;
		TempTokens.PopBack();

		ParseType(PropertyInfo.Type, TempTokens);
		TempClassInfo.Properties.EmplaceBack(std::move(PropertyInfo));
	}
}
