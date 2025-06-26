#pragma once

#include "Core.h"

namespace Bloodshot::Shared
{
	enum class ELanguage : uint8_t
	{
		English = 0,
		Russian = 1,
		LanguageCount = 2
	};

	class FTranslator final
	{
	public:
		~FTranslator() {}

		NODISCARD static FTranslator& GetInstance();

		NODISCARD FORCEINLINE static ELanguage GetCurrentLanguage()
		{
			return GetInstance().CurrentLanguage;
		}

		FORCEINLINE static void SetLanguage(const ELanguage InLanguage)
		{
			FTranslator& Instance = GetInstance();
			bool& bEnglish = Instance.bEnglish;

			switch (InLanguage)
			{
				case ELanguage::English:
				{
					bEnglish = true;
					return;
				}
				case ELanguage::Russian:
				{
					Instance.CurrentDictionary = &Instance.Dictionaries[static_cast<uint8_t>(InLanguage)];
					break;
				}
				case ELanguage::LanguageCount:
				{
					BS_ASSERT(false, "FTranslator::SetLanguage: Passed LanguageCount instead of language");
					return;
				}
			}
			bEnglish = false;
		}

		FORCEINLINE static void AddPhrase(FStringView InSourcePhrase,
			FStringView InTranslatedPhrase,
			const ELanguage InLanguage)
		{
			(void)GetInstance().Dictionaries[static_cast<uint8_t>(InLanguage)].FindOrAdd(InSourcePhrase, InTranslatedPhrase);
		}

		FORCEINLINE static void AddPhraseToCurrentDictionary(FStringView InSourcePhrase,
			FStringView InTranslatedPhrase)
		{
			(void)GetInstance().CurrentDictionary->FindOrAdd(InSourcePhrase, InTranslatedPhrase);
		}

		NODISCARD FORCEINLINE static const char* Translate(FStringView InPhrase)
		{
			FTranslator& Instance = GetInstance();

			if (Instance.bEnglish)
			{
				return InPhrase.GetData();
			}

			return (*Instance.CurrentDictionary)[InPhrase].GetData();
		}

	private:
		FTranslator() {}

		using FLanguageDictionary = TMap<FString, FString>;
		FLanguageDictionary Dictionaries[static_cast<uint8_t>(ELanguage::LanguageCount)];
		FLanguageDictionary* CurrentDictionary = nullptr;
		ELanguage CurrentLanguage = ELanguage::English;

		bool bEnglish = true;
	};
}

#define TRANSLATE(InString) ::Bloodshot::Shared::FTranslator::Translate(##InString##)

namespace Bloodshot::Shared::Private
{
	template<typename... ArgTypes>
	FString TranslateFormatHelper(FStringView InFormat, ArgTypes... Args)
	{
		return String::Format(InFormat, (TRANSLATE(Args), ...));
	}
}

#define TRANSLATE_FORMAT(InFormat, ...) ::Bloodshot::Shared::Private::TranslateFormatHelper(InFormat, ##__VA_ARGS__)
