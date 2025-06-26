#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FVertex final
	{
	public:
		glm::vec3 Position = glm::vec3(0.f);
		glm::vec2 TextureCoords = glm::vec2(0.f);
		glm::vec3 Normal = glm::vec3(0.f);
		glm::vec3 Tangent = glm::vec3(0.f);
	};
}
