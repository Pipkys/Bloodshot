#include "ImGuiHeader.h"

THIRD_PARTY_INCLUDES_START
#include <imgui_widgets.cpp>
THIRD_PARTY_INCLUDES_END

namespace ImGui::Ext
{
	bool ImageButton(const char* StringID,
		ImTextureID TextureID,
		const ImVec2& ImageSize,
		const ImVec2& UV0,
		const ImVec2& UV1,
		const ImVec4& BackgroundColor,
		const ImVec4& TintColor,
		const ImVec4& TintHoveredColor,
		const ImVec4& TintActiveColor)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;
		ImGuiID id = window->GetID(StringID);

		const ImVec2 padding = g.Style.FramePadding;
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImageSize + padding * 2.0f);
		ItemSize(bb);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, 0);

		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderNavCursor(bb, id);
		RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, g.Style.FrameRounding));
		if (BackgroundColor.w > 0.0f)
			window->DrawList->AddRectFilled(bb.Min + padding, bb.Max - padding, GetColorU32(BackgroundColor));
		window->DrawList->AddImage(TextureID, bb.Min + padding, bb.Max - padding, UV0, UV1, GetColorU32((held && hovered) ? TintActiveColor : hovered ? TintHoveredColor : TintColor));

		return pressed;
	}

	bool ImageTextButton(const ImTextureID InTextureID,
		const char* InLabel,
		const ImVec2 InImageSize,
		const ImVec2 InUV0,
		const ImVec2 InUV1,
		const int InFramePadding,
		const ImVec4& InBackgroundColor,
		const ImVec4& InTintColor)
	{
		ImGuiWindow* const Window = GetCurrentWindow();
		if (Window->SkipItems)
		{
			return false;
		}

		ImVec2 ImageSize = InImageSize;
		if (ImageSize.x <= 0 && ImageSize.y <= 0)
		{
			ImageSize.x = ImageSize.y = ImGui::GetTextLineHeightWithSpacing();
		}
		else
		{
			if (ImageSize.x <= 0)
			{
				ImageSize.x = ImageSize.y;
			}
			else if (ImageSize.y <= 0)
			{
				ImageSize.y = ImageSize.x;
			}
			ImageSize *= Window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
		}

		ImGuiContext& Context = *GImGui;
		const ImGuiStyle& Style = Context.Style;

		const ImGuiID LabelID = Window->GetID(InLabel);
		const ImVec2 LabelSize = ImGui::CalcTextSize(InLabel, NULL, true);
		const bool bHasText = LabelSize.x > 0;

		const float InnerSpacing = bHasText ? ((InFramePadding >= 0)
			? (float)InFramePadding
			: (Style.ItemInnerSpacing.x)) : 0.f;

		const ImVec2 Padding = (InFramePadding >= 0)
			? ImVec2((float)InFramePadding, (float)InFramePadding)
			: Style.FramePadding;

		const ImVec2 TotalSizeWithoutPadding(ImageSize.x + InnerSpacing + LabelSize.x,
			ImageSize.y > LabelSize.y ? ImageSize.y : LabelSize.y);

		const ImRect BoundingBox(Window->DC.CursorPos,
			Window->DC.CursorPos + TotalSizeWithoutPadding + Padding * 2.f);

		ImVec2 LabelStart(0.f, 0.f);
		LabelStart = Window->DC.CursorPos + Padding;
		if (ImageSize.y < LabelSize.y)
		{
			LabelStart.y += (LabelSize.y - ImageSize.y) * 0.5f;
		}

		const ImRect ImageBoundingBox(LabelStart, LabelStart + ImageSize);
		LabelStart = Window->DC.CursorPos + Padding;
		LabelStart.x += ImageSize.x + InnerSpacing;
		if (ImageSize.y > LabelSize.y)
		{
			LabelStart.y += (ImageSize.y - LabelSize.y) * 0.5f;
		}

		ItemSize(BoundingBox);
		if (!ItemAdd(BoundingBox, LabelID))
		{
			return false;
		}

		bool bButtonHovered;
		bool bButtonHeld;
		bool bButtonPressed = ButtonBehavior(BoundingBox, LabelID, &bButtonHovered, &bButtonHeld);

		const ImU32 Color = GetColorU32((bButtonHovered && bButtonHeld)
			? ImGuiCol_ButtonActive
			: bButtonHovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

		RenderFrame(BoundingBox.Min,
			BoundingBox.Max,
			Color,
			true,
			ImClamp((float)ImMin(Padding.x, Padding.y), 0.0f, Style.FrameRounding));

		if (InBackgroundColor.w > 0.0f)
		{
			Window->DrawList->AddRectFilled(ImageBoundingBox.Min,
				ImageBoundingBox.Max,
				GetColorU32(InBackgroundColor));
		}

		Window->DrawList->AddImage(InTextureID,
			ImageBoundingBox.Min,
			ImageBoundingBox.Max,
			InUV0,
			InUV1,
			GetColorU32(InTintColor));

		if (LabelSize.x > 0)
		{
			ImGui::RenderText(LabelStart, InLabel);
		}

		return bButtonPressed;
	}

	bool DragFloat(const char* InLabel,
		float* const InValue,
		const float InValueSpeed,
		const float InValueMin, 
		const float InValueMax,
		const char* InFormat,
		const ImGuiSliderFlags InFlags,
		bool& bInInputTriggered)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(InLabel);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(InLabel, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		const bool temp_input_allowed = (InFlags & ImGuiSliderFlags_NoInput) == 0;
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
			return false;

		if (InFormat == NULL)
			InFormat = DataTypeGetInfo(ImGuiDataType_Float)->PrintFmt;

		const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.ItemFlags);
		bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
		if (!temp_input_is_active)
		{
			const bool clicked = hovered && IsMouseClicked(0, ImGuiInputFlags_None, id);
			const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2 && TestKeyOwner(ImGuiKey_MouseLeft, id));
			const bool make_active = (clicked || double_clicked || g.NavActivateId == id);
			if (make_active && (clicked || double_clicked))
				SetKeyOwner(ImGuiKey_MouseLeft, id);
			if (make_active && temp_input_allowed)
				if ((clicked && g.IO.KeyCtrl) || double_clicked || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
					temp_input_is_active = true;

			if (g.IO.ConfigDragClickToInputText && temp_input_allowed && !temp_input_is_active)
				if (g.ActiveId == id && hovered && g.IO.MouseReleased[0] && !IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * DRAG_MOUSE_THRESHOLD_FACTOR))
				{
					g.NavActivateId = id;
					g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
					temp_input_is_active = true;
				}

			if (make_active)
				memcpy(&g.ActiveIdValueOnActivation, InValue, DataTypeGetInfo(ImGuiDataType_Float)->Size);

			if (make_active && !temp_input_is_active)
			{
				SetActiveID(id, window);
				SetFocusID(id, window);
				FocusWindow(window);
				g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			}
		}

		if (temp_input_is_active)
		{
			bool clamp_enabled = false;
			if ((InFlags & ImGuiSliderFlags_ClampOnInput) && (InValueMin != NULL || InValueMax != NULL))
			{
				const int clamp_range_dir = (InValueMin != NULL && InValueMax != NULL) ? DataTypeCompare(ImGuiDataType_Float, &InValueMin, &InValueMax) : 0; // -1 when *InValueMin < *InValueMax, == 0 when *InValueMin == *InValueMax
				if (InValueMin == NULL || InValueMax == NULL || clamp_range_dir < 0)
					clamp_enabled = true;
				else if (clamp_range_dir == 0)
					clamp_enabled = DataTypeIsZero(ImGuiDataType_Float, &InValueMin) ? ((InFlags & ImGuiSliderFlags_ClampZeroRange) != 0) : true;
			}
			bInInputTriggered = true;
			return TempInputScalar(frame_bb, id, InLabel, ImGuiDataType_Float, InValue, InFormat, clamp_enabled ? &InValueMin : NULL, clamp_enabled ? &InValueMax : NULL);
		}

		bInInputTriggered = false;
		const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		RenderNavCursor(frame_bb, id);
		RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

		const bool value_changed = DragBehavior(id, ImGuiDataType_Float, InValue, InValueSpeed, &InValueMin, &InValueMax, InFormat, InFlags);
		if (value_changed)
			MarkItemEdited(id);

		char value_buf[64];
		const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), ImGuiDataType_Float, InValue, InFormat);
		if (g.LogEnabled)
			LogSetNextTextDecoration("{", "}");
		RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

		if (label_size.x > 0.0f)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), InLabel);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, InLabel, g.LastItemData.StatusFlags | (temp_input_allowed ? ImGuiItemStatusFlags_Inputable : 0));
		return value_changed;
	}
}
