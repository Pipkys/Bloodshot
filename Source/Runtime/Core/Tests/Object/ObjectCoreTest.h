#pragma once

#ifdef BS_WITH_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#endif

#include "Object/Object.h"
#include "Object/ObjectMacros.h"

namespace Bloodshot::Test
{
	BSCLASS();
	class FTestObject final : public IObject
	{
		GENERATED_BODY();

	private:
		BSPROPERTY();
		size_t Number;

		BSPROPERTY();
		FString String;

		BSPROPERTY();
		TArray<size_t> Array;

		BSFUNCTION();
		NODISCARD FORCEINLINE size_t GetNumber(const size_t Number)
		{
			return Number;
		}

		BSFUNCTION();
		NODISCARD FORCEINLINE FString GetString(const FString& String)
		{
			return String;
		}
	};
}
