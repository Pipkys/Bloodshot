#pragma once

#include "Core.h"

namespace Bloodshot::HeaderTool
{
	struct FPropertyInfo final
	{
		FString Type;
		FString Name;

		bool bStatic = false;
		//bool bSerialized = false;
		//bool bReplicated = false;
	};

	struct FParameterInfo final
	{
		FString Type;
		FString Name;
	};

	struct FFunctionInfo final
	{
		FString ReturnType;
		FString Name;
		TArray<FParameterInfo> Parameters;

		bool bStatic = false;
		bool bConst = false;
		bool bNoexcept = false;
	};

	struct FClassInfo final
	{
		FString Name;
		FString Namespace;
		TArray<FString> BaseClassNames;
		TArray<FPropertyInfo> Properties;
		TArray<FFunctionInfo> Functions;

		bool bAbstract = false;
		bool bFinal = false;
		bool bDerived = false;
	};
}
