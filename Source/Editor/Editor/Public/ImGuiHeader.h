#pragma once

#include "Core.h"

THIRD_PARTY_INCLUDES_START
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
THIRD_PARTY_INCLUDES_END

namespace ImGui::Ext
{
	FORCEINLINE bool Begin(const char* InWindowName,
		bool& bInWindowFocused,
		ImGuiFocusedFlags InWindowFocusedFlags = 0,
		bool* const bInWindowOpened = nullptr,
		ImGuiWindowFlags InWindowFlags = 0)
	{
		const bool bResult = ImGui::Begin(InWindowName, bInWindowOpened, InWindowFlags);
		bInWindowFocused = ImGui::IsWindowFocused(InWindowFocusedFlags);
		return bResult;
	}

	FORCEINLINE bool Begin(const char* InWindowName,
		const ImVec2& InWindowMinSize,
		bool& bInWindowFocused,
		ImGuiFocusedFlags InWindowFocusedFlags = 0,
		bool* const bInWindowOpened = nullptr,
		ImGuiWindowFlags InWindowFlags = 0)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, InWindowMinSize);
		const bool bResult = ImGui::Begin(InWindowName, bInWindowOpened, InWindowFlags);
		ImGui::PopStyleVar();
		bInWindowFocused = ImGui::IsWindowFocused(InWindowFocusedFlags);
		return bResult;
	}

	bool ImageButton(const char* StringID,
		ImTextureID TextureID,
		const ImVec2& ImageSize,
		const ImVec2& UV0 = ImVec2(0.f, 0.f),
		const ImVec2& UV1 = ImVec2(1.f, 1.f),
		const ImVec4& BackgroundColor = ImVec4(0.f, 0.f, 0.f, 0.f),
		const ImVec4& TintColor = ImVec4(1.f, 1.f, 1.f, 1.f),
		const ImVec4& TintHoveredColor = ImVec4(1.f, 1.f, 1.f, 1.f),
		const ImVec4& TintActiveColor = ImVec4(1.f, 1.f, 1.f, 1.f));

	bool ImageTextButton(const ImTextureID InTextureID,
		const char* InLabel,
		const ImVec2 InImageSize = ImVec2(0, 0),
		const ImVec2 InUV0 = ImVec2(0, 0),
		const ImVec2 InUV1 = ImVec2(1, 1),
		const int InFramePadding = -1,
		const ImVec4& InBackgroundColor = ImVec4(0, 0, 0, 0),
		const ImVec4& InTintColor = ImVec4(1, 1, 1, 1));

	bool DragFloat(const char* InLabel,
		float* const InValue,
		const float InValueSpeed,
		const float InValueMin,
		const float InValueMax,
		const char* InFormat,
		const ImGuiSliderFlags InFlags,
		bool& bInInputTriggered);

	NODISCARD FORCEINLINE ImVec2 GetMouseDragDelta(ImGuiMouseButton button, float lock_threshold = -1.f)
	{
		ImGuiContext& g = *GImGui;
		IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
		if (lock_threshold < 0.0f)
			lock_threshold = g.IO.MouseDragThreshold;
		if (g.IO.MouseDown[button] || g.IO.MouseReleased[button])
			if (g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold)
				if (IsMousePosValid(&g.IO.MousePos) && IsMousePosValid(&g.IO.MouseClickedPos[button]))
					return ImVec2((float)((int)g.IO.MousePos.x - (int)g.IO.MouseClickedPos[button].x),
					(float)((int)g.IO.MousePos.y - (int)g.IO.MouseClickedPos[button].y));
		return ImVec2(0.0f, 0.0f);
	}
}
