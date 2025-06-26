#pragma once

#include "Core.h"

#include "Component.h"

namespace Bloodshot
{
	BSCLASS();
	class FLightComponent final : public IComponent
	{
		GENERATED_BODY();

	public:
		BSPROPERTY();
		glm::vec4 Color = glm::vec4(1.f, 1.f, 1.f, 1.f);
	};
}




