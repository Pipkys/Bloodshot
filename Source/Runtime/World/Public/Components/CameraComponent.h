#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) // Variable 'ViewMatrix' is uninitialized

#include "Core.h"

#include "Camera.h"
#include "Component.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	BSCLASS();
	class FCameraComponent final : public IComponent
	{
		GENERATED_BODY();

	public:
		FORCEINLINE FCameraComponent(const FCameraSettings& Settings = FCameraSettings())
			: Camera(Settings)
		{
		}

		NODISCARD FORCEINLINE ECameraProjectionMode GetProjectionMode() const
		{
			return Camera.GetProjectionMode();
		}

		NODISCARD FORCEINLINE const glm::mat4& GetProjectionMatrix() const
		{
			return Camera.GetProjectionMatrix();
		}

		NODISCARD FORCEINLINE const glm::mat4& GetViewMatrix() const
		{
			const glm::vec3& NewPosition = TransformComponent->GetPosition();
			if (OldPosition != NewPosition)
			{
				Camera.UpdateViewMatrix(TransformComponent->GetTransform());
				OldPosition = NewPosition;
			}
			return Camera.GetViewMatrix();
		}

		NODISCARD FORCEINLINE float GetFov() const
		{
			return Camera.GetFov();
		}

		NODISCARD FORCEINLINE float GetAspectRatio() const
		{
			return Camera.GetAspectRatio();
		}

		NODISCARD FORCEINLINE float GetNearPlain() const
		{
			return Camera.GetNearPlain();
		}

		NODISCARD FORCEINLINE float GetFarPlain() const
		{
			return Camera.GetFarPlain();
		}

		NODISCARD FORCEINLINE float GetLeft() const
		{
			return Camera.GetLeft();
		}

		NODISCARD FORCEINLINE float GetRight() const
		{
			return Camera.GetRight();
		}

		NODISCARD FORCEINLINE float GetBottom() const
		{
			return Camera.GetBottom();
		}

		NODISCARD FORCEINLINE float GetTop() const
		{
			return Camera.GetTop();
		}

		NODISCARD FORCEINLINE FCamera* GetNativeCamera()
		{
			return &Camera;
		}

		NODISCARD FORCEINLINE const FCamera* GetNativeCamera() const
		{
			return &Camera;
		}

		FORCEINLINE void SetProjectionMode(const ECameraProjectionMode NewValue)
		{
			Camera.SetProjectionMode(NewValue);
		}

		FORCEINLINE void SetFov(const float NewValue)
		{
			Camera.SetFov(NewValue);
		}

		FORCEINLINE void SetAspectRatio(const float NewValue)
		{
			Camera.SetAspectRatio(NewValue);
		}

		FORCEINLINE void SetAspectRatio(const float Width, const float Height)
		{
			Camera.SetAspectRatio(Width, Height);
		}

		FORCEINLINE void SetNearPlain(const float NewValue)
		{
			Camera.SetNearPlain(NewValue);
		}

		FORCEINLINE void SetFarPlain(const float NewValue)
		{
			Camera.SetFarPlain(NewValue);
		}

		FORCEINLINE void SetLeft(const float NewValue)
		{
			Camera.SetLeft(NewValue);
		}

		FORCEINLINE void SetRight(const float NewValue)
		{
			Camera.SetRight(NewValue);
		}

		FORCEINLINE void SetBottom(const float NewValue)
		{
			Camera.SetBottom(NewValue);
		}

		FORCEINLINE void SetTop(const float NewValue)
		{
			Camera.SetTop(NewValue);
		}

	private:
		virtual void BeginPlay() override;

		FTransformComponent* TransformComponent = nullptr;
		FCamera Camera;
		mutable glm::vec3 OldPosition;
	};
}

#pragma warning(pop)
