#pragma once

#include "Core.h"

#include "Token.h"

namespace Bloodshot::HeaderTool
{
	class FTokenizer final
	{
	public:
		TArray<FToken> Tokenize(FStringView SourceCode);

	private:
		FString SourceCode;
		size_t Position = 0;

		char Get() const;
		char Consume();
		char Peek() const;

		FToken Tokenize(const ETokenType Type);

		void Clear();
	};
}
