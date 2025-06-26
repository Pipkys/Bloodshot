#pragma once

#include "Core.h"

#include <initializer_list>

namespace Bloodshot::Shared
{
	struct FCmdOption
	{
		FString Name;
		FString Value;
	};

	class FCmdParser
	{
	public:
		FORCEINLINE FCmdParser(const int InArgCount, char** InArgs)
		{
			for (size_t i = 0; i < InArgCount; ++i)
			{
				Args.EmplaceBack(InArgs[i]);
			}
		}

		FORCEINLINE FCmdParser(const int InArgCount, char** InArgs, const TArray<FString>& InOptions)
			: FCmdParser(InArgCount, InArgs)
		{
			PossibleOptions = InOptions;
		}

		FORCEINLINE FCmdParser(const int InArgCount, char** InArgs, TArray<FString>&& InOptions)
			: FCmdParser(InArgCount, InArgs)
		{
			PossibleOptions = std::move(InOptions);
		}

		NODISCARD FORCEINLINE const TArray<FString>& GetPassedArgs() const
		{
			return Args;
		}

		NODISCARD FORCEINLINE const FCmdOption& GetOption(const size_t Index) const
		{
			BS_ASSERT(Index < ParsedOptions.GetSize(),
				"FCmdParser::GetOption: Trying to get not added option by Index: {}", Index);
			return ParsedOptions[Index];
		}

		NODISCARD FORCEINLINE const FCmdOption& GetOption(FStringView OptionName) const
		{
			for (const FCmdOption& Option : ParsedOptions)
			{
				if (Option.Name == OptionName)
				{
					return Option;
				}
			}

			BS_ASSERT(false, "FCmdParser::GetOption: Trying to get not added option: {}", OptionName);
		}

		NODISCARD FORCEINLINE const FString& GetOptionName(const size_t Index) const
		{
			return GetOption(Index).Name;
		}

		NODISCARD FORCEINLINE const FString& GetOptionValue(const size_t Index) const
		{
			return GetOption(Index).Value;
		}

		NODISCARD FORCEINLINE const FString& GetOptionValue(FStringView OptionName) const
		{
			return GetOption(OptionName).Value;
		}

		NODISCARD FORCEINLINE const TArray<FCmdOption>& GetOptions() const
		{
			return ParsedOptions;
		}

		NODISCARD FORCEINLINE const FString& GetErrorMessage() const
		{
			return ErrorMessage;
		}

		NODISCARD FORCEINLINE bool HasOption(FStringView OptionName) const
		{
			return PossibleOptions.Contains(OptionName)
				&& ParsedOptions.FindByPredicate([OptionName](const FCmdOption& Option)
			{
				return Option.Name == OptionName;
			}) != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool HasOptions(std::initializer_list<FStringView> OptionNames) const
		{
			for (FStringView OptionName : OptionNames)
			{
				if (!HasOption(OptionName))
				{
					return false;
				}
			}
			return true;
		}

		NODISCARD FORCEINLINE bool HasAllOptions() const
		{
			if (ParsedOptions.GetSize() < PossibleOptions.GetSize())
			{
				return false;
			}
			for (const FString& OptionName : PossibleOptions)
			{
				if (!IsOptionParsed(OptionName))
				{
					return false;
				}
			}
			return true;
		}

		FORCEINLINE void AddOption(FStringView OptionName)
		{
			PossibleOptions.EmplaceBack(OptionName);
		}

		FORCEINLINE void RemoveOption(FStringView OptionName)
		{
			PossibleOptions.Remove(OptionName);
		}

		FORCEINLINE void Parse()
		{
			for (const FString& Arg : Args)
			{
				FCmdOption Option;
				if (TryParseOption(Arg, Option))
				{
					ParsedOptions.EmplaceBack(std::move(Option));
				}
			}
		}

		FORCEINLINE void BuildErrorMessage(std::initializer_list<std::initializer_list<size_t>> OptionPacks = {})
		{
			FString PassedArgsString = "Passed: (";
			if (Args.GetSize())
			{
				for (const FString& Arg : Args)
				{
					PassedArgsString += Arg + " ";
				}
				PassedArgsString.RemoveLast();
			}
			PassedArgsString += ")";

			FString ParsedOptionsString = "Parsed: (";
			if (ParsedOptions.GetSize())
			{
				for (const FCmdOption& Option : ParsedOptions)
				{
					ParsedOptionsString += Option.Value + " ";
				}
				ParsedOptionsString.RemoveLast();
			}
			ParsedOptionsString += ")";

			FString PossibleOptionsString;
			if (!OptionPacks.size())
			{
				PossibleOptionsString = String::Format("Pass {} options in any order: (", PossibleOptions.GetSize());
				for (const FString& Option : PossibleOptions)
				{
					PossibleOptionsString += String::Format("[{}:(Value)], ", Option);
				}
				PossibleOptionsString.RemoveLast(2);
				PossibleOptionsString += "), other args will be ignored";
			}
			else
			{
				PossibleOptionsString = String::Format("Pass 1 of {} possible option packs with options placed in any order: (", PossibleOptions.GetSize());
				for (size_t i = 0; i < OptionPacks.size(); ++i)
				{
					PossibleOptionsString += String::Format("{} - (", i);
					std::initializer_list<size_t> OptionPack = OptionPacks.begin()[i];
					for (size_t OptionIndex : OptionPack)
					{
						PossibleOptionsString += String::Format("[{}:(Value)], ", GetOptionName(OptionIndex));
					}
					PossibleOptionsString.RemoveLast(2);
					PossibleOptionsString += "), ";
				}
				PossibleOptionsString.RemoveLast(2);
				PossibleOptionsString += ")";
			}

			ErrorMessage = PassedArgsString + " " + ParsedOptionsString + " " + PossibleOptionsString;
		}

		FORCEINLINE void BuildErrorMessage(std::initializer_list<std::initializer_list<FStringView>> OptionPacks = {})
		{
			FString PassedArgsString = "Passed: (";
			if (Args.GetSize())
			{
				for (const FString& Arg : Args)
				{
					PassedArgsString += Arg + " ";
				}
				PassedArgsString.RemoveLast();
			}
			PassedArgsString += ")";

			FString ParsedOptionsString = "Parsed: (";
			if (ParsedOptions.GetSize())
			{
				for (const FCmdOption& Option : ParsedOptions)
				{
					ParsedOptionsString += Option.Value + " ";
				}
				ParsedOptionsString.RemoveLast();
			}
			ParsedOptionsString += ")";

			FString PossibleOptionsString;
			if (!OptionPacks.size())
			{
				PossibleOptionsString = String::Format("Pass {} options in any order: (", PossibleOptions.GetSize());
				for (const FString& Option : PossibleOptions)
				{
					PossibleOptionsString += String::Format("[{}:(Value)], ", Option);
				}
				PossibleOptionsString.RemoveLast(2);
				PossibleOptionsString += "), other args will be ignored";
			}
			else
			{
				PossibleOptionsString = String::Format("Pass 1 of {} possible option packs with options placed in any order: (", PossibleOptions.GetSize());
				for (size_t i = 0; i < OptionPacks.size(); ++i)
				{
					PossibleOptionsString += String::Format("{} - (", i);
					std::initializer_list<FStringView> OptionPack = OptionPacks.begin()[i];
					for (FStringView OptionName : OptionPack)
					{
						PossibleOptionsString += String::Format("[{}:(Value)], ", OptionName);
					}
					PossibleOptionsString.RemoveLast(2);
					PossibleOptionsString += "), ";
				}
				PossibleOptionsString.RemoveLast(2);
				PossibleOptionsString += ")";
			}

			ErrorMessage = PassedArgsString + " " + ParsedOptionsString + " " + PossibleOptionsString;
		}

	private:
		TArray<FString> Args;
		TArray<FString> PossibleOptions;
		TArray<FCmdOption> ParsedOptions;
		FString ErrorMessage;

		NODISCARD FORCEINLINE bool IsOptionParsed(FStringView OptionName) const
		{
			return ParsedOptions.FindByPredicate([OptionName](const FCmdOption& Option)
			{
				return Option.Name == OptionName;
			}) != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool TryParseOption(FStringView String, FCmdOption& OutOption) const
		{
			for (const FString& PossibleOption : PossibleOptions)
			{
				const FString& Option = String::Format("{}:", PossibleOption);
				const size_t Index = String.Find(Option);
				if (Index != IReservedValues::NoneIndex && Index == 0)
				{
					OutOption.Name = PossibleOption;
					OutOption.Value = String.SubStr(Option.GetSize());
					return true;
				}
			}
			return false;
		}
	};
}
