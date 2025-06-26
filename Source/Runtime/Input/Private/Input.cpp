#include "Input.h"

namespace Bloodshot
{
	FInput& FInput::GetInstance()
	{
		static FInput Instance;
		return Instance;
	}
}
