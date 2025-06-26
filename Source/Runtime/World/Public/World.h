#pragma once

#include "Core.h"

#include "Level.h"

namespace Bloodshot
{
	class FWorld final
	{
		friend class Private::Launch::IEngineContext;
		friend class Private::Launch::FEngineEditorContext;
		friend class Private::Launch::FEngineGameContext;

	public:
		NODISCARD static FWorld& GetInstance();

		NODISCARD FORCEINLINE static FLevel* GetCurrentLevel()
		{
			return GetInstance().CurrentLevel;
		}

		static void AddLevel(FStringView InName);
		static void LoadLevel(const size_t Index);
		static void LoadLevel(FStringView InName);
		static void SetStartingLevel(const size_t Index);

	private:
		FWorld() {}

		TArray<FLevel*> Levels;
		FLevel* CurrentLevel = nullptr;

		static void BeginPlay();
		static void EndPlay();
		static void Tick(float DeltaTime);
	};
}
