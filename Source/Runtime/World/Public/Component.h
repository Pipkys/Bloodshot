#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) // Variable 'Owner' is uninitialized

#include "Core.h"

namespace Bloodshot
{
	class FEntity;

	BSCLASS();
	class IComponent : public IObject
	{
		friend class FEntity;

		GENERATED_BODY();

	public:
		BSPROPERTY();
		bool bActive = true;

		NODISCARD FORCEINLINE FEntity* GetOwner()
		{
			return Owner;
		}

		NODISCARD FORCEINLINE const FEntity* GetOwner() const
		{
			return Owner;
		}

	protected:
		FEntity* Owner;

		virtual void BeginPlay() {}
		virtual void EndPlay() {}
	};

	template<typename T>
	concept IsComponent = std::is_base_of_v<IComponent, T>;
}

#pragma warning(pop)
