#include "EntitySelectionBuffer.h"

namespace Bloodshot
{
	FEntitySelectionBuffer& FEntitySelectionBuffer::GetInstance()
	{
		static FEntitySelectionBuffer Instance;
		return Instance;
	}
}
