#pragma once

#include "Containers/Tuple.h"

namespace Bloodshot
{
	template<typename FirstElementType, typename SecondElementType>
	using TPair = TTuple<FirstElementType, SecondElementType>;
}
