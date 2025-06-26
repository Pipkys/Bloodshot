#include "Level.h"
#include "World.h"

namespace Bloodshot
{
	FLevel* FLevel::Self()
	{
		FLevel* const Self = FWorld::GetCurrentLevel();
		BS_CHECK(Self);
		return Self;
	}
}
