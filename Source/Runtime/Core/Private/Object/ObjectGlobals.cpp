#include "Object/ObjectGlobals.h"
#include "Misc/AssertionMacros.h"

namespace Bloodshot
{
	namespace Private::Object
	{
		FObjectCore& FObjectCore::GetInstance()
		{
			static FObjectCore Instance;
			return Instance;
		}

		void FObjectCore::DeleteObject(IObject* const Object)
		{
			BS_PROFILE_FUNCTION();
			FObjectCore& Instance = GetInstance();
			Instance.Objects.Remove(Object);
			const uint32_t ObjectTypeID = Object->TypeID;
			FMemory::Free(Object->ObjectClass);
			DestructElement(Object);
			FObjectAllocatorView ObjectAllocator = FindObjectAllocator(ObjectTypeID);
			ObjectAllocator.Deallocate(Object);
		}

		FObjectAllocatorView FObjectCore::FindObjectAllocator(const uint32_t ObjectTypeID)
		{
			BS_PROFILE_FUNCTION();
			FObjectAllocatorMap& ObjectAllocators = GetInstance().ObjectAllocators;
			FObjectAllocatorView* const AllocatorView = ObjectAllocators.Find(ObjectTypeID);
			BS_CHECK(AllocatorView);
			return *AllocatorView;
		}
	}

	void DeleteObject(IObject* const Object)
	{
		Private::Object::FObjectCore::DeleteObject(Object);
	}
}
