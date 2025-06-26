#pragma once

#include "Core.h"

#include "OpenGL/ImGuiHeader.h"
#include "Texture.h"

#include <functional>
#include <memory>

namespace Bloodshot::Launcher
{
	class FWidgetState final
	{
		friend class FGui;

	public:
		NODISCARD FORCEINLINE bool IsHovered() const
		{
			return bHovered;
		}

		NODISCARD FORCEINLINE bool IsClicked() const
		{
			return bClicked;
		}

	private:
		bool bHovered = false;
		bool bClicked = false;
	};

	struct IWidget abstract
	{
		mutable FWidgetState State;
		ImGuiMouseCursor HoveredCursor = ImGuiMouseCursor_Arrow;
	};

	struct FText final : IWidget
	{
		FString Value = "";
		ImFont* Font = nullptr;

		ImVec4 Color = {};
		ImVec4 HoveredColor = {};
	};

	struct FButton final : IWidget
	{
		FString Text = "";
		ImFont* Font = nullptr;

		ImVec4 TextColor = {};
		ImVec4 HoveredTextColor = {};

		ImVec4 Color = {};
		ImVec4 ActiveColor = {};
		ImVec4 HoveredColor = {};

		std::function<void()> OnClickEvent = []() {};
	};

	struct FImage final : IWidget
	{
		FTexture Texture = {};
		ImVec2 Size = {};

		ImVec4 Color = {};
		ImVec4 HoveredColor = {};
	};

	struct FImageButton final : IWidget
	{
		FString UniqueID = "";

		FTexture Texture = {};
		ImVec2 Size = {};

		ImVec4 ImageColor = {};
		ImVec4 HoveredImageColor = {};

		ImVec4 ButtonColor = {};
		ImVec4 ActiveButtonColor = {};
		ImVec4 HoveredButtonColor = {};

		std::function<void()> OnClickEvent = []() {};
	};

	struct FInputTextBox final : IWidget
	{
		FString Label = "";
		FString Hint = "";
		ImFont* Font = nullptr;

		TSharedPtr<char[]> Buffer = nullptr;
		size_t BufferSize = 0;

		ImVec4 Color = {};
		ImVec4 ActiveColor = {};
		ImVec4 HoveredColor = {};

		ImGuiInputTextFlags Flags = 0;
		ImGuiInputTextCallback Callback = 0;
		void* CallbackData = nullptr;
	};

	struct FLine final
	{
		ImVec4 Color = ImVec4(1.f, 1.f, 1.f, 1.f);
		float Thickness = 1.f;
	};

	struct FTopPanel final
	{
		FButton CloseButton = {};
		FLine CloseButtonIcon[2] = {};

		FButton MaximizeButton = {};
		FLine MaximizeButtonIcon[4] = {};

		FButton MinimizeButton = {};
		FLine MinimizeButtonIcon = {};
	};

	class FWidgetGroup
	{
	public:
		NODISCARD FORCEINLINE size_t GetSize() const
		{
			return Texts.GetSize()
				+ Buttons.GetSize()
				+ Images.GetSize()
				+ ImageButtons.GetSize()
				+ InputTextBoxes.GetSize()
				+ Lines.GetSize();
		}

		NODISCARD FORCEINLINE FText& GetText(const FString& UniqueID)
		{
			return Texts[UniqueID];
		}

		NODISCARD FORCEINLINE const FText& GetText(const FString& UniqueID) const
		{
			return Texts[UniqueID];
		}

		NODISCARD FORCEINLINE FButton& GetButton(const FString& UniqueID)
		{
			return Buttons[UniqueID];
		}

		NODISCARD FORCEINLINE const FButton& GetButton(const FString& UniqueID) const
		{
			return Buttons[UniqueID];
		}

		NODISCARD FORCEINLINE FImage& GetImage(const FString& UniqueID)
		{
			return Images[UniqueID];
		}

		NODISCARD FORCEINLINE const FImage& GetImage(const FString& UniqueID) const
		{
			return Images[UniqueID];
		}

		NODISCARD FORCEINLINE FImageButton& GetImageButton(const FString& UniqueID)
		{
			return ImageButtons[UniqueID];
		}

		NODISCARD FORCEINLINE const FImageButton& GetImageButton(const FString& UniqueID) const
		{
			return ImageButtons[UniqueID];
		}

		NODISCARD FORCEINLINE FInputTextBox& GetInputTextBox(const FString& UniqueID)
		{
			return InputTextBoxes[UniqueID];
		}

		NODISCARD FORCEINLINE const FInputTextBox& GetInputTextBox(const FString& UniqueID) const
		{
			return InputTextBoxes[UniqueID];
		}

		NODISCARD FORCEINLINE FLine& GetLine(const FString& UniqueID)
		{
			return Lines[UniqueID];
		}

		NODISCARD FORCEINLINE const FLine& GetLine(const FString& UniqueID) const
		{
			return Lines[UniqueID];
		}

		FORCEINLINE void SetText(const FString& UniqueID, FText&& Text)
		{
			Texts.Emplace(UniqueID, std::move(Text));
		}

		FORCEINLINE void SetButton(const FString& UniqueID, FButton&& Button)
		{
			Buttons.Emplace(UniqueID, std::move(Button));
		}

		FORCEINLINE void SetImage(const FString& UniqueID, FImage&& Image)
		{
			Images.Emplace(UniqueID, std::move(Image));
		}

		FORCEINLINE void SetImageButton(const FString& UniqueID, FImageButton&& ImageButton)
		{
			ImageButtons.Emplace(UniqueID, std::move(ImageButton));
		}

		FORCEINLINE void SetInputTextBox(const FString& UniqueID, FInputTextBox&& InputTextBox)
		{
			InputTextBoxes.Emplace(UniqueID, std::move(InputTextBox));
		}

		FORCEINLINE void SetLine(const FString& UniqueID, FLine&& Line)
		{
			Lines.Emplace(UniqueID, std::move(Line));
		}

		FORCEINLINE void AddText(FString&& UniqueID, FText&& Text)
		{
			Texts.Emplace(std::move(UniqueID), std::move(Text));
		}

		FORCEINLINE void AddButton(FString&& UniqueID, FButton&& Button)
		{
			Buttons.Emplace(std::move(UniqueID), std::move(Button));
		}

		FORCEINLINE void AddImage(FString&& UniqueID, FImage&& Image)
		{
			Images.Emplace(std::move(UniqueID), std::move(Image));
		}

		FORCEINLINE void AddImageButton(FString&& UniqueID, FImageButton&& ImageButton)
		{
			ImageButtons.Emplace(std::move(UniqueID), std::move(ImageButton));
		}

		FORCEINLINE void AddInputTextBox(FString&& UniqueID, FInputTextBox&& InputTextBox)
		{
			InputTextBoxes.Emplace(std::move(UniqueID), std::move(InputTextBox));
		}

		FORCEINLINE void AddLine(FString&& UniqueID, FLine&& Line)
		{
			Lines.Emplace(std::move(UniqueID), std::move(Line));
		}

	private:
		TMap<FString, FText> Texts;
		TMap<FString, FButton> Buttons;
		TMap<FString, FImage> Images;
		TMap<FString, FImageButton> ImageButtons;
		TMap<FString, FInputTextBox> InputTextBoxes;
		TMap<FString, FLine> Lines;
	};

	class FPage final : public FWidgetGroup
	{
	public:
		ImVec2 TopOffset = ImVec2(24, 36);
		ImVec2 BottomOffset = ImVec2(12, 12);

		NODISCARD FORCEINLINE FWidgetGroup& GetWidgetGroup(const FString& UniqueID)
		{
			return WidgetGroups[UniqueID];
		}

		NODISCARD FORCEINLINE const FWidgetGroup& GetWidgetGroup(const FString& UniqueID) const
		{
			return WidgetGroups[UniqueID];
		}

		NODISCARD FORCEINLINE bool ContainsWidgetGroup(const FString& UniqueID) const
		{
			return WidgetGroups.Contains(UniqueID);
		}

		FORCEINLINE void SetWidgetGroup(const FString& UniqueID, FWidgetGroup&& Group)
		{
			WidgetGroups.Emplace(UniqueID, std::move(Group));
		}

		FORCEINLINE void AddWidgetGroup(FString&& UniqueID, FWidgetGroup&& Group)
		{
			WidgetGroups.Emplace(std::move(UniqueID), std::move(Group));
		}

	private:
		TMap<FString, FWidgetGroup> WidgetGroups;
	};

	class FWindow
	{
	public:
		ImVec2 TopPanelOffset = ImVec2(12, 12);

		NODISCARD FORCEINLINE FTopPanel& GetTopPanel()
		{
			return TopPanel;
		}

		NODISCARD FORCEINLINE const FTopPanel& GetTopPanel() const
		{
			return TopPanel;
		}

		NODISCARD FORCEINLINE TMap<FString, FPage>& GetPages()
		{
			return Pages;
		}

		NODISCARD FORCEINLINE const TMap<FString, FPage>& GetPages() const
		{
			return Pages;
		}

		NODISCARD FORCEINLINE FPage& GetPage(const FString& UniqueID)
		{
			return Pages[UniqueID];
		}

		NODISCARD FORCEINLINE const FPage& GetPage(const FString& UniqueID) const
		{
			return Pages[UniqueID];
		}

		FORCEINLINE void SetTopPanel(FTopPanel&& InTopPanel)
		{
			TopPanel = std::move(InTopPanel);
		}

		FORCEINLINE void SetPage(const FString& UniqueID, FPage&& Page)
		{
			Pages.Emplace(UniqueID, std::move(Page));
		}

	private:
		FTopPanel TopPanel;
		TMap<FString, FPage> Pages;
	};
}
