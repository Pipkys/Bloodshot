#pragma once

#include "Core.h"

namespace Bloodshot
{
	struct FMaterialProperties final
	{
		glm::vec3 BaseColor = glm::vec3(1.f);
		float Metallic = 0.f;
		glm::vec3 Specular = glm::vec3(0.f);
		float Roughness = 0.f;
		float Anisotropy = 0.f;
		glm::vec3 EmissiveColor = glm::vec3(0.f);
		float Opacity = 0.f;
		glm::vec3 Normal = glm::vec3(0.f, 0.f, 1.f);
		glm::vec3 Tangent = glm::vec3(1.f, 0.f, 0.f);
		glm::vec3 Ambient = glm::vec3(0.f);
		glm::vec3 Diffuse = glm::vec3(0.f);
	};
}
