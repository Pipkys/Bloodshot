#include "Entity.h"
#include "Level.h"

namespace Bloodshot
{
	void FEntity::Destroy()
	{
		FLevel::Destroy(this);
	}
}
