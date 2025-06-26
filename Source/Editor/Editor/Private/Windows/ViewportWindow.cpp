#include "Windows/ViewportWindow.h"
#include "Editor.h"
#include "EditorConfig.h"
#include "ImGuiHeader.h"
#include "RHI.h"
#include "Translator.h"
#include "WorldRenderer.h"

namespace Bloodshot
{
	FViewportWindow::FViewportWindow()
		: Size(glm::vec2(IEditorConfig::EditorWindowMinSize.x, IEditorConfig::EditorWindowMinSize.y))
	{
		FFrameBufferSettings FrameBufferSettings;
		FrameBufferSettings.Attachments =
		{
			EFrameBufferTextureFormat::RGBA8,
			EFrameBufferTextureFormat::RED_INTEGER,
			EFrameBufferTextureFormat::Depth
		};
		FrameBufferSettings.Width = (uint32_t)IEditorConfig::EditorWindowMinSize.x;
		FrameBufferSettings.Height = (uint32_t)IEditorConfig::EditorWindowMinSize.y;
		FrameBuffer = FRHI::CreateFrameBuffer(FrameBufferSettings);
	}

	FViewportWindow::~FViewportWindow()
	{
		FRHI::DestroyFrameBuffer(FrameBuffer);
	}

	void FViewportWindow::Input()
	{
		Camera.Input(Position, Size);
	}

	void FViewportWindow::Tick()
	{
		const FFrameBufferSettings& FrameBufferSettings = FrameBuffer->GetSettings();
		if (Size.x > 0.0f && Size.y > 0.0f
			&& (FrameBufferSettings.Width != Size.x || FrameBufferSettings.Height != Size.y))
		{
			FrameBuffer->Resize((uint32_t)Size.x, (uint32_t)Size.y);
			Camera.GetNativeCamera()->SetAspectRatio(Size.x, Size.y);
		}
	}

	void FViewportWindow::Render()
	{
		FrameBuffer->Bind();
		FRHI::ClearBackground();
		FWorldRenderer::Render(Camera.GetNativeCamera(), Camera.Transform.GetPosition());
		FrameBuffer->Unbind();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::Ext::Begin(TRANSLATE("Viewport"), bFocused))
		{
			ImGui::GetIO().MouseDrawCursor = 0;
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
			const ImVec2& ViewportSize = ImGui::GetContentRegionAvail();
			const ImVec2& ViewportPosition = ImGui::GetCursorScreenPos();
			Position = glm::vec2(ViewportPosition.x, ViewportPosition.y);
			Size = glm::vec2(ViewportSize.x, ViewportSize.y);
			ImGui::Image(FrameBuffer->GetColorAttachmentID(), ViewportSize);
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
}
