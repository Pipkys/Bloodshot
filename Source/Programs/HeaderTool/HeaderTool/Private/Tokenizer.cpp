#include "Tokenizer.h"
#include "Keywords.h"

#include <fstream>
#include <sstream>

namespace Bloodshot::HeaderTool
{
	TArray<FToken> FTokenizer::Tokenize(FStringView SourceCode)
	{
		Clear();
		this->SourceCode = SourceCode;
		TArray<FToken> Result;
		while (Position < SourceCode.GetSize())
		{
			const char CurrentChar = Get();

			if (std::isspace(CurrentChar))
			{
				Consume();
				continue;
			}

			FToken Token;
			if (CurrentChar == ':' && Peek() == ':')
			{
				Token = Tokenize(ETokenType::ScopeResolutionOperator);
			}
			else if (std::isalpha(CurrentChar) || CurrentChar == '_')
			{
				Token = Tokenize((ETokenType)((uint8_t)ETokenType::Identifier | (uint8_t)ETokenType::Keyword));
			}
			else if (std::isdigit(CurrentChar))
			{
				Token = Tokenize(ETokenType::NumberLiteral);
			}
			else if (CurrentChar == '"')
			{
				Token = Tokenize(ETokenType::StringLiteral);
			}
			else if (CurrentChar == '/' && Peek() == '/')
			{
				Token = Tokenize(ETokenType::Comment);
			}
			else if (IsOperator(CurrentChar))
			{
				Token = Tokenize(ETokenType::Operator);
			}
			else if (IsSeparator(CurrentChar))
			{
				Token = Tokenize(ETokenType::Separator);
			}
			else
			{
				Token = Tokenize(ETokenType::Unknown);
			}

			Result.EmplaceBack(std::move(Token));
		}

		return Result;
	}

	char FTokenizer::Get() const
	{
		return SourceCode[Position];
	}

	char FTokenizer::Consume()
	{
		return SourceCode[Position++];
	}

	char FTokenizer::Peek() const
	{
		return (Position + 1 < SourceCode.GetSize()) ? SourceCode[Position + 1] : '\0';
	}

	FToken FTokenizer::Tokenize(const ETokenType Type)
	{
		const size_t SourceCodeSize = SourceCode.GetSize();

		FToken Token;
		Token.Type = Type;

		switch (Type)
		{
			case (ETokenType)((uint8_t)ETokenType::Keyword | (uint8_t)ETokenType::Identifier) :
			{
				while (Position < SourceCodeSize && (std::isalnum(Get()) || Get() == '_'))
				{
					Token.Value += Consume();
				}
				Token.Type = IsKeyword(Token.Value) ? ETokenType::Keyword : ETokenType::Identifier;
				break;
			}
			case ETokenType::NumberLiteral:
			{
				while (Position < SourceCodeSize && std::isdigit(Get()))
				{
					Token.Value += Consume();
				}
				break;
			}
			case ETokenType::StringLiteral:
			{
				Token.Value += Consume();
				while (Position < SourceCodeSize && Get() != '"')
				{
					Token.Value += Consume();
				}
				if (Position < SourceCodeSize)
				{
					Token.Value += Consume();
				}
				break;
			}
			case ETokenType::Operator:
			{
				Token.Value = Consume();
				break;
			}
			case ETokenType::ScopeResolutionOperator:
			{
				Consume();
				Consume();
				Token.Value = "::";
				break;
			}
			case ETokenType::Separator:
			{
				Token.Value = Consume();
				break;
			}
			case ETokenType::Comment:
			{
				while (Position < SourceCodeSize && Get() != '\n')
				{
					Token.Value += Consume();
				}
				break;
			}
			case ETokenType::Unknown:
			{
				Token.Value = Consume();
				break;
			}
		}

		return Token;
	}

	void FTokenizer::Clear()
	{
		SourceCode.Clear();
		Position = 0;
	}
}
