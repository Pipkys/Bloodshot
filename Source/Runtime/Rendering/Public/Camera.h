#pragma once

#include "Core.h"

#include "RHI.h"

namespace Bloodshot
{
	enum class ECameraProjectionMode : uint8_t
	{
		Orthographic = 0,
		Perspective
	};

	struct FCameraSettings final
	{
		ECameraProjectionMode ProjectionMode = ECameraProjectionMode::Perspective;

		float Fov = 45.f;
		float AspectRatio = (float)FRHI::GetWindowWidth() / (float)FRHI::GetWindowHeight();
		float Near = 0.001f;
		float Far = 1000.f;
		float Left = 0.f;
		float Right = (float)FRHI::GetWindowWidth();
		float Bottom = 0.f;
		float Top = (float)FRHI::GetWindowHeight();
	};

	class FCamera
	{
	public:
		FORCEINLINE FCamera(const FCameraSettings& Settings = FCameraSettings())
			: Settings(Settings)
		{
			UpdateProjectionMatrix();
		}

		NODISCARD FORCEINLINE ECameraProjectionMode GetProjectionMode() const
		{
			return Settings.ProjectionMode;
		}

		NODISCARD FORCEINLINE const glm::mat4& GetProjectionMatrix() const
		{
			if (bDirty)
			{
				UpdateProjectionMatrix();
				bDirty = false;
			}

			return ProjectionMatrix;
		}

		NODISCARD FORCEINLINE const glm::mat4& GetViewMatrix() const
		{
			return ViewMatrix;
		}

		NODISCARD FORCEINLINE float GetFov() const
		{
			return Settings.Fov;
		}

		NODISCARD FORCEINLINE float GetAspectRatio() const
		{
			return Settings.AspectRatio;
		}

		NODISCARD FORCEINLINE float GetNearPlain() const
		{
			return Settings.Near;
		}

		NODISCARD FORCEINLINE float GetFarPlain() const
		{
			return Settings.Far;
		}

		NODISCARD FORCEINLINE float GetLeft() const
		{
			return Settings.Left;
		}

		NODISCARD FORCEINLINE float GetRight() const
		{
			return Settings.Right;
		}

		NODISCARD FORCEINLINE float GetBottom() const
		{
			return Settings.Bottom;
		}

		NODISCARD FORCEINLINE float GetTop() const
		{
			return Settings.Top;
		}

		FORCEINLINE void SetProjectionMode(const ECameraProjectionMode NewValue)
		{
			ECameraProjectionMode& ProjectionMode = Settings.ProjectionMode;
			if (ProjectionMode != NewValue)
			{
				ProjectionMode = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetFov(const float NewValue)
		{
			float& Fov = Settings.Fov;
			if (Fov != NewValue)
			{
				Fov = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetAspectRatio(const float NewValue)
		{
			float& AspectRatio = Settings.AspectRatio;
			if (AspectRatio != NewValue)
			{
				AspectRatio = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetAspectRatio(const float Width, const float Height)
		{
			float& AspectRatio = Settings.AspectRatio;
			const float NewValue = Width / Height;
			if (AspectRatio != NewValue)
			{
				AspectRatio = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetNearPlain(const float NewValue)
		{
			float& Near = Settings.Near;
			if (Near != NewValue)
			{
				Near = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetFarPlain(const float NewValue)
		{
			float& Far = Settings.Far;
			if (Far != NewValue)
			{
				Far = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetLeft(const float NewValue)
		{
			float& Left = Settings.Left;
			if (Left != NewValue)
			{
				Left = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetRight(const float NewValue)
		{
			float& Right = Settings.Right;
			if (Right != NewValue)
			{
				Right = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetBottom(const float NewValue)
		{
			float& Bottom = Settings.Bottom;
			if (Bottom != NewValue)
			{
				Bottom = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void SetTop(const float NewValue)
		{
			float& Top = Settings.Top;
			if (Top != NewValue)
			{
				Top = NewValue;
				bDirty = true;
			}
		}

		FORCEINLINE void UpdateProjectionMatrix() const
		{
			ProjectionMatrix = Settings.ProjectionMode == ECameraProjectionMode::Perspective
				? glm::perspective(Settings.Fov, Settings.AspectRatio, Settings.Near, Settings.Far)
				: glm::ortho(Settings.Left, Settings.Right, Settings.Bottom, Settings.Top);
		}

		FORCEINLINE void UpdateViewMatrix(const FTransform& Transform) const
		{
			const glm::vec3& Position = Transform.GetPosition();
			ViewMatrix = glm::lookAt(Position, Position + Transform.GetForwardVector(), Transform.GetUpVector());
		}

	private:
		FCameraSettings Settings;
		mutable glm::mat4 ProjectionMatrix;
		mutable glm::mat4 ViewMatrix;
		mutable bool bDirty = false;
	};
}
