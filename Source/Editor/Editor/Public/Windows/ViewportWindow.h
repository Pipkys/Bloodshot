#pragma once

#include "Core.h"

#include "Camera.h"
#include "EditorWindow.h"
#include "FrameBuffer.h"
#include "ImGuiHeader.h"
#include "Input.h"

namespace Bloodshot
{
	class FViewportCamera final
	{
		friend class FViewportWindow;

	public:
		NODISCARD FORCEINLINE FCamera* GetNativeCamera()
		{
			return &Camera;
		}

		NODISCARD FORCEINLINE const FCamera* GetNativeCamera() const
		{
			return &Camera;
		}

	private:
		FORCEINLINE FViewportCamera()
		{
			UpdateViewMatrix();
		}

		FTransform Transform;
		FCamera Camera;
		glm::vec3 OldPosition;
		glm::vec3 OldRotation;
		bool bMouseCaptureFirstFrame = true;
		bool bMouseCaptured = true;

		FORCEINLINE void UpdateViewMatrix()
		{
			OldPosition = Transform.GetPosition();
			OldRotation = Transform.GetRotation();
			Camera.UpdateViewMatrix(Transform);
		}

		FORCEINLINE void Input(const glm::vec2& ViewportPosition, const glm::vec2& ViewportSize)
		{
			const ImVec2 MousePosition = ImGui::GetMousePos();
			if (MousePosition.x >= ViewportPosition.x
				&& MousePosition.y >= ViewportPosition.y
				&& MousePosition.x <= ViewportPosition.x + ViewportSize.x
				&& MousePosition.y <= ViewportPosition.y + ViewportSize.y)
			{
				if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
				{
					bMouseCaptureFirstFrame = !bMouseCaptured ? true : false;
					bMouseCaptured = true;
				}
				else
				{
					bMouseCaptured = false;
					bMouseCaptureFirstFrame = false;
				}
			}

			if (bMouseCaptureFirstFrame)
			{
				ImGui::TeleportMousePos(ImVec2(ViewportPosition.x + ViewportSize.x / 2.f,
					ViewportPosition.y + ViewportSize.y / 2.f));
				ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
				bMouseCaptureFirstFrame = false;
			}

			if (bMouseCaptured)
			{
				float Speed = 0.013f;
				if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
				{
					Speed *= 3.f;
				}
				if (ImGui::IsKeyDown(ImGuiKey_W))
				{
					Transform.Translate(Transform.GetForwardVector() * Speed);
				}
				if (ImGui::IsKeyDown(ImGuiKey_A))
				{
					Transform.Translate(-Transform.GetRightVector() * Speed);
				}
				if (ImGui::IsKeyDown(ImGuiKey_S))
				{
					Transform.Translate(-Transform.GetForwardVector() * Speed);
				}
				if (ImGui::IsKeyDown(ImGuiKey_D))
				{
					Transform.Translate(Transform.GetRightVector() * Speed);
				}
				if (ImGui::IsKeyDown(ImGuiKey_E))
				{
					Transform.Translate(IVector3Constants::Up * Speed);
				}
				if (ImGui::IsKeyDown(ImGuiKey_Q))
				{
					Transform.Translate(-IVector3Constants::Up * Speed);
				}

				const ImVec2 MouseDelta = ImGui::Ext::GetMouseDragDelta(ImGuiMouseButton_Right);
				if (MouseDelta.x != 0.f || MouseDelta.y != 0.f)
				{
					ImGui::TeleportMousePos(ImVec2(ViewportPosition.x + ViewportSize.x / 2.f,
						ViewportPosition.y + ViewportSize.y / 2.f));
					ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
				}

				const float Sensitivity = 0.15f;
				const float RotationX = Sensitivity * MouseDelta.x;
				const float RotationY = Sensitivity * MouseDelta.y;

				if (Transform.GetRotation().y + RotationY <= 90.f
					&& Transform.GetRotation().y + RotationY >= -90.f)
				{
					Transform.Rotate(glm::vec3(0.f, RotationY, 0.f));
				}

				Transform.Rotate(glm::vec3(0.f, 0.f, RotationX));
			}

			if (OldPosition != Transform.GetPosition() || OldRotation != Transform.GetRotation())
			{
				UpdateViewMatrix();
			}
		}
	};

	class FViewportWindow final : public IEditorWindow
	{
		friend class FEditor;

		FViewportWindow();
		~FViewportWindow();

		FViewportCamera Camera;
		FFrameBuffer* FrameBuffer;
		glm::vec2 Position;
		glm::vec2 Size;

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;
	};
}
