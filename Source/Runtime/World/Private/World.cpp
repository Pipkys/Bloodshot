#include "World.h"

namespace Bloodshot
{
	FWorld& FWorld::GetInstance()
	{
		static FWorld Instance;
		return Instance;
	}

	void FWorld::AddLevel(FStringView InName)
	{
		BS_ASSERT(!FEngineState::IsSimulating(), "FWorld::AddLevel: Trying to create level in runtime");
		FLevel* const Level = NewObject<FLevel>(InName);
		GetInstance().Levels.EmplaceBack(Level);
	}

	void FWorld::LoadLevel(const size_t Index)
	{
		FWorld& Instance = GetInstance();
		TArray<FLevel*>& Levels = Instance.Levels;

		if (Index >= Levels.GetSize())
		{
			BS_LOG(Error, "Trying to load not existing level with index: {}", Index);
			return;
		}

		FLevel*& CurrentLevel = Instance.CurrentLevel;
		if (CurrentLevel && FEngineState::IsSimulating())
		{
			CurrentLevel->EndPlay();
		}
		CurrentLevel = Levels[Index];
		if (FEngineState::IsSimulating())
		{
			CurrentLevel->BeginPlay();
		}
	}

	void FWorld::LoadLevel(FStringView InName)
	{
		FWorld& Instance = GetInstance();
		TArray<FLevel*>& Levels = Instance.Levels;
		FLevel* NewLevel = nullptr;

		for (FLevel* const Level : Levels)
		{
			if (Level->Name == InName)
			{
				NewLevel = Level;
			}
		}

		if (!NewLevel)
		{
			BS_LOG(Error, "Trying to load not existing level with name: {}", InName);
			return;
		}

		FLevel*& CurrentLevel = Instance.CurrentLevel;
		if (CurrentLevel && FEngineState::IsSimulating())
		{
			CurrentLevel->EndPlay();
		}
		CurrentLevel = NewLevel;
		if (FEngineState::IsSimulating())
		{
			CurrentLevel->BeginPlay();
		}
	}

	void FWorld::SetStartingLevel(const size_t Index)
	{
		FWorld& Instance = GetInstance();
		TArray<FLevel*> Levels = Instance.Levels;
		BS_ASSERT(!FEngineState::IsSimulating(), "FWorld::SetStartingLevel: Trying to set starting level in runtime");
		BS_ASSERT(Index >= Levels.GetSize(), "FWorld::SetStartingLevel: Trying to change starting level to not existing level");
		Instance.CurrentLevel = Levels[Index];
	}

	void FWorld::BeginPlay()
	{
		FWorld& Instance = GetInstance();
		FLevel* const CurrentLevel = Instance.CurrentLevel;
		BS_ASSERT(CurrentLevel, "FWorld::BeginPlay: Current level is not set");
		BS_LOG(Debug, "Beginning play on level with index: {}...", Instance.Levels.Find(CurrentLevel));
		CurrentLevel->BeginPlay();
	}

	void FWorld::EndPlay()
	{
		FWorld& Instance = GetInstance();
		FLevel* const CurrentLevel = Instance.CurrentLevel;
		BS_ASSERT(CurrentLevel, "FWorld::EndPlay: Current level is not set");
		BS_LOG(Debug, "Ending play on FLevel with index: {}...", Instance.Levels.Find(CurrentLevel));
		CurrentLevel->EndPlay();
	}

	void FWorld::Tick(float DeltaTime)
	{
		FLevel* const CurrentLevel = GetInstance().CurrentLevel;
		BS_ASSERT(CurrentLevel, "FWorld::Tick: Current level is not set");
		CurrentLevel->Tick(DeltaTime);
	}
}
