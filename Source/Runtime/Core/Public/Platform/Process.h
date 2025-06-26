#pragma once

#include "Containers/StringView.h"

namespace Bloodshot
{
	struct IProcess final
	{
		static void Create(FStringView Command);
	};
}
