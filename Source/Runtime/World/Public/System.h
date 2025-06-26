#pragma once

#include "Core.h"

namespace Bloodshot
{
	BSCLASS();
	class ISystem : public IObject
	{
		friend class FLevel;

		GENERATED_BODY();

	public:
		virtual ~ISystem() = default;

		BSPROPERTY();
		bool bEnabled = true;

	protected:
		virtual void FixedTick() {}
		virtual void Tick(float DeltaTime) {}
		virtual void LateTick(float DeltaTime) {}
	};

	template<typename T>
	concept IsSystem = std::is_base_of_v<ISystem, T>;
}
