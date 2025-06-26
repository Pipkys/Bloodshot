#include "Containers/String.h"
#include "Containers/StringView.h"
#include "String/StringManipulations.h"

namespace Bloodshot::String
{
	void ParseTokens(FStringView InView, FStringView InDelimiter, const std::function<void(FStringView)>& InVisitor)
	{
		const char* ViewData = InView.GetData();
		size_t ViewSize = InView.GetSize();
		const char* DelimiterData = InDelimiter.GetData();
		const size_t DelimiterSize = InDelimiter.GetSize();
		size_t SameCharCount = 0;
		FStringView TokenView;
		for (size_t i = 0; i < ViewSize;)
		{
			if (SameCharCount == DelimiterSize)
			{
				TokenView = FStringView(ViewData, i - DelimiterSize);
				InVisitor(TokenView);
				ViewData += i;
				ViewSize -= i;
				SameCharCount = 0;
				i = 0;
				continue;
			}
			else if (ViewData[i] == DelimiterData[SameCharCount])
			{
				++SameCharCount;
			}
			++i;
		}
		TokenView = FStringView(ViewData, ViewSize);
		InVisitor(TokenView);
	}

	void Replace(FString& InString, FStringView InSubStrToReplace, FStringView InNewSubStr)
	{
		FString Result;
		ParseTokens(InString, InSubStrToReplace, [&Result, &InNewSubStr](FStringView InToken)
		{
			Result.Append(InToken).Append(InNewSubStr);
		});
		Result.RemoveAt(Result.GetSize() - InNewSubStr.GetSize(), InNewSubStr.GetSize());
		InString = std::move(Result);
		//const char* StringData = InString.GetData();
		//size_t StringSize = InString.GetSize();
		//const char* SubStrToReplaceData = InSubStrToReplace.GetData();
		//const size_t SubStrToReplaceSize = InSubStrToReplace.GetSize();
		//FString Result;
		//Result.Reserve(StringSize);
		//size_t SameCharCount = 0;
		//for (size_t i = 0; i < StringSize;)
		//{
		//	if (SameCharCount == SubStrToReplaceSize)
		//	{
		//		Result.Append(FStringView(StringData, i - SubStrToReplaceSize));
		//		Result.Append(InNewSubStr);
		//		StringData += i;
		//		StringSize -= i;
		//		SameCharCount = 0;
		//		i = 0;
		//		continue;
		//	}
		//	else if (StringData[i] == SubStrToReplaceData[SameCharCount])
		//	{
		//		++SameCharCount;
		//	}
		//	++i;
		//}
		//InString = std::move(Result);
	}
}
