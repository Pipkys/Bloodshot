#include "Components/CameraComponent.h"
#include "Entity.h"

namespace Bloodshot
{
	void FCameraComponent::BeginPlay()
	{
		TransformComponent = Owner->GetComponent<FTransformComponent>();
		OldPosition = TransformComponent->GetPosition();
		Camera.UpdateViewMatrix(TransformComponent->GetTransform());
	}
}
