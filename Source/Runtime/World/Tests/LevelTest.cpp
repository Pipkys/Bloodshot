#ifdef BS_WITH_AUTOMATION_TESTS

#include "Core.h"

#include "Level.h"

namespace Bloodshot
{
	TEST_CASE_NAMED(FLevelInstantiateDestroyTest)
	{
		FLogger::SetLogLevelFlags(~ELogLevel::Trace);
		{
			FEntity* const Entity = FLevel::Instantiate();
			TEST_ASSERT(Entity->bActive);
			FLevel::Destroy(Entity);
		}
		{
			TArray<FEntity*> Entities = FLevel::Instantiate(100);
			TEST_ASSERT(Entities.GetSize() == 100);
			FEntity* const Entity = Entities[99];
			TEST_ASSERT(Entity->bActive);
			FLevel::Destroy(Entities);
		}
		FLogger::SetLogLevelFlags(ELogLevel::All);
	}
	
	TEST_CASE_NAMED(FLevelInstantiateDestroyStressTest)
	{
		FLogger::SetLogLevelFlags(~ELogLevel::Trace);
		TArray<FEntity*> Entities = FLevel::Instantiate(100000);
		TEST_ASSERT(Entities.GetSize() == 100000);
		FEntity* const Entity = Entities[99999];
		TEST_ASSERT(Entity->bActive);
		FLevel::Destroy(Entities);
		FLogger::SetLogLevelFlags(ELogLevel::All);
	}
}

#endif