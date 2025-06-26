#pragma once

#include "Translator.h"

namespace Bloodshot::Shared
{
	FTranslator& FTranslator::GetInstance()
	{
		static FTranslator Instance;
		return Instance;
	}
}
