#pragma once

#include "Core.h"

#include "Component.h"

namespace Bloodshot
{
	BSCLASS();
	class FTransformComponent final : public IComponent
	{
		GENERATED_BODY();

	public:
		NODISCARD FORCEINLINE const FTransform& GetTransform() const
		{
			return Transform;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetPosition() const
		{
			return Transform.GetPosition();
		}

		NODISCARD FORCEINLINE const glm::quat& GetRotation() const
		{
			return Transform.GetRotation();
		}

		NODISCARD FORCEINLINE const glm::vec3& GetScale() const
		{
			return Transform.GetScale();
		}

		NODISCARD FORCEINLINE const glm::vec3& GetForwardVector() const
		{
			return Transform.GetForwardVector();
		}

		NODISCARD FORCEINLINE const glm::vec3& GetRightVector() const
		{
			return Transform.GetRightVector();
		}

		NODISCARD FORCEINLINE const glm::vec3& GetUpVector() const
		{
			return Transform.GetUpVector();
		}

		NODISCARD FORCEINLINE void SetPosition(const glm::vec3& NewPosition)
		{
			Transform.SetPosition(NewPosition);
		}

		NODISCARD FORCEINLINE void SetRotation(const glm::vec3& NewRotation)
		{
			Transform.SetRotation(NewRotation);
		}

		NODISCARD FORCEINLINE void SetScale(const glm::vec3& NewScale)
		{
			Transform.SetScale(NewScale);
		}

		NODISCARD FORCEINLINE void Translate(const glm::vec3& Translation)
		{
			Transform.Translate(Translation);
		}

		NODISCARD FORCEINLINE void Rotate(const glm::vec3& Rotation)
		{
			Transform.Rotate(Rotation);
		}

		NODISCARD FORCEINLINE void Scale(const glm::vec3& InScale)
		{
			Transform.Scale(InScale);
		}

		NODISCARD FORCEINLINE glm::mat4 CalculateModelMatrix() const
		{
			return Transform.CalculateModelMatrix();
		}

	private:
		BSPROPERTY(Serialized, Replicated);
		FTransform Transform;
	};
}
