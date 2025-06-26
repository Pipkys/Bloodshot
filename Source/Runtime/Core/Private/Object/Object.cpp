#include "Object/Object.h"
#include "Object/Class.h"
#include "Object/ObjectGlobals.h"

namespace Bloodshot
{
	FClass* IObject::GetPrivateStaticClass()
	{
		static FClass Instance("IObject",
			"::Bloodshot",
			nullptr,
			0,
			nullptr,
			0,
			nullptr,
			0,
			true,
			false,
			false,
			sizeof(IObject),
			0);

		return &Instance;
	}

	bool IObject::IsA(const FClass* const InClass) const
	{
		return String::IsEqual(ObjectClass->Name, InClass->Name) 
			&& String::IsEqual(ObjectClass->Namespace, InClass->Namespace);
	}

	template<>
	FClass* Private::Object::FObjectCore::ConstructClass<IObject>(IObject* const Object, void* const Memory)
	{
		return Memory
			? new(Memory) FClass("IObject",
			"::Bloodshot",
			nullptr,
			0,
			nullptr,
			0,
			nullptr,
			0,
			true,
			false,
			false,
			sizeof(IObject),
			0)
			: new FClass("IObject",
			"::Bloodshot",
			nullptr,
			0,
			nullptr,
			0,
			nullptr,
			0,
			true,
			false,
			false,
			sizeof(IObject),
			0);
	}
}
