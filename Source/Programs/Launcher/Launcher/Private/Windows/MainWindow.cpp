#include "Windows/MainWindow.h"
#include "IsValidPath.h"
#include "Launcher.h"
#include "Settings.h"
#include "Translator.h"
#include "Windows/AddProjectWindow.h"
#include "Windows/NewProjectWindow.h"
#include "Windows/OpenProjectWindow.h"

namespace Bloodshot::Launcher
{
	NODISCARD FMainWindow& FMainWindow::GetInstance()
	{
		static FMainWindow Instance;
		return Instance;
	}

	void FMainWindow::Draw()
	{
		FGui::BeginWindow("Bloodshot Engine Launcher",
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse,
			FSettings::MinWindowSize);
		{
			FWindow& Window = FGui::GetWindow("Main");
			if (CurrentPage == "Main")
			{
				const FPage& Page = Window.GetPage("Main");
				const ImVec2& TopOffset = Page.TopOffset;

				FGui::SetFramePaddingMultiplier(4);
				FGui::SetCursorPosition(TopOffset);

				const ImVec2& FramePadding = FGui::GetFramePadding();

				FGui::MoveCursor(-FramePadding);

				const FImageButton& LogoImageButton = Page.GetImageButton("Logo");
				const ImVec2& LogoImageButtonSize = FGui::CalculateSize(LogoImageButton);
				const FButton& GithubLinkButton = Page.GetButton("GithubLink");
				const ImVec2& GithubLinkButtonSize = FGui::CalculateSize(GithubLinkButton);
				const float CenteredButtonsLocalPositionY = FGui::GetCursorPosition().y
					+ LogoImageButtonSize.y * 0.5f
					- GithubLinkButtonSize.y * 0.5f;

				FGui::Draw(LogoImageButton);

				FGui::KeepLine();
				FGui::MoveCursorX(-FramePadding.x);
				FGui::SetCursorPositionY(CenteredButtonsLocalPositionY);
				FGui::Draw(GithubLinkButton);
				FGui::UpdateState(2);

				FGui::KeepLine();
				FGui::MoveCursorX(TopOffset.x - FramePadding.x / 2);
				FGui::SetCursorPositionY(CenteredButtonsLocalPositionY);

				const ImVec2& WindowPosition = FGui::GetWindowPosition();
				const ImVec2& WindowSize = FGui::GetWindowSize();
				const ImVec2& WindowCornerGlobalPosition = WindowPosition + WindowSize;
				const ImVec2& ProjectsButtonLocalPosition = FGui::GetCursorPosition();
				const ImVec2& ProjectsButtonGlobalPosition = WindowPosition + ProjectsButtonLocalPosition;

				const FButton& ProjectsButton = Page.GetButton("Projects");
				const ImVec2& ProjectsButtonSize = FGui::CalculateSize(ProjectsButton);

				FGui::Draw(Page.GetLine("HorizontalMainSeparator"),
					ImVec2(ProjectsButtonGlobalPosition.x, ProjectsButtonGlobalPosition.y + ProjectsButtonSize.y),
					ImVec2(WindowCornerGlobalPosition.x - 5.f, ProjectsButtonGlobalPosition.y + ProjectsButtonSize.y));
				FGui::Draw(Page.GetLine("VerticalMainSeparator"),
					ProjectsButtonGlobalPosition,
					ImVec2(ProjectsButtonGlobalPosition.x, WindowCornerGlobalPosition.y - 5.f));
				FGui::Draw(ProjectsButton);
				FGui::UpdateState();

				//FGui::KeepLine();
				//FGui::SetCursorPositionY(CenteredButtonsLocalPositionY);
				//
				//const FButton& ChangelogButton = Page.GetButton("Changelog");
				//
				//FGui::Draw(ChangelogButton);
				//FGui::UpdateState();
				FGui::SetFramePaddingMultiplier(1);

				FGui::SetCursorPosition(ProjectsButtonLocalPosition);
				FGui::MoveCursorY(ProjectsButtonSize.y);

				if (CurrentTab == "Projects")
				{
					FProjectsTab::GetInstance().Draw();
				}
				else
				{
					FChangelogTab::GetInstance().Draw();
				}

				const ImVec2& BottomOffset = Page.BottomOffset;
				const FImageButton& SettingsImageButton = Page.GetImageButton("Settings");
				const ImVec2& SettingsImageButtonSize = FGui::CalculateSize(SettingsImageButton);

				const FButton& SettingsButton = Page.GetButton("Settings");
				const ImVec2& SettingsButtonSize = FGui::CalculateSize(SettingsButton);

				FGui::SetCursorPositionX(ProjectsButtonLocalPosition.x / 2.f
					- SettingsImageButtonSize.x / 2.f
					- SettingsButtonSize.x / 2.f);
				FGui::SetCursorPositionY(WindowSize.y - BottomOffset.y - SettingsImageButtonSize.y);
				FGui::Draw(SettingsImageButton);

				FGui::KeepLine();
				FGui::MoveCursorY(-2.f);
				FGui::Draw(SettingsButton);
				FGui::UpdateState(2);

				FGui::SetCursorPosition(ProjectsButtonLocalPosition);
				FGui::MoveCursorY(ProjectsButtonSize.y);
				FGui::MoveCursor(-1.f, -1.f);
			}
			else if (CurrentPage == "Settings")
			{
				FPage& Page = Window.GetPage("Settings");
				const ImVec2& TopOffset = Page.TopOffset;

				const ImVec2& WindowSize = FGui::GetWindowSize();
				const FButton& BackButton = Page.GetButton("Back");
				const ImVec2& BackButtonSize = ImVec2(WindowSize.x, 40.f);

				FGui::SetFramePaddingMultiplier(4);
				FGui::SetCursorPosition(0.f, TopOffset.y);
				FGui::Draw(BackButton, BackButtonSize);

				const FImage& BackImage = Page.GetImage("Back");
				const ImVec2& BackImageSize = FGui::CalculateSize(BackImage);

				FGui::SetCursorPosition(TopOffset);
				FGui::MoveCursorY(BackButtonSize.y / 2.f - BackImageSize.y / 2.f);
				FGui::Draw(BackImage);

				const ImVec2& FramePadding = FGui::GetFramePadding();

				FGui::KeepLine();
				FGui::SetCursorPosition(TopOffset);
				FGui::MoveCursorX(BackImageSize.x + FramePadding.x);
				FGui::MoveCursorY(BackButtonSize.y / 2.f - BackButton.Font->FontSize / 2);

				const ImVec2& CursorGlobalPosition = FGui::GetWindowPosition() + FGui::GetCursorPosition();

				FGui::Draw(Page.GetLine("Back"),
					ImVec2(CursorGlobalPosition.x, CursorGlobalPosition.y),
					ImVec2(CursorGlobalPosition.x, CursorGlobalPosition.y + BackButton.Font->FontSize - 1.f));

				FGui::MoveCursorX(FramePadding.x);

				const FText& BackText = Page.GetText("Back");

				FGui::SetCursorPositionY(TopOffset.y + BackButtonSize.y / 2.f - FGui::CalculateSize(BackText).y / 2.f);
				FGui::Draw(BackText);
				FGui::UpdateState(3);
				FGui::SetFramePaddingMultiplier(1);

				FGui::SetCursorPosition(TopOffset);
				FGui::MoveCursorY(BackButtonSize.y);
				FGui::MoveCursorY(BackButtonSize.y / 2.f);

				const FText& ProjectsPathText = Page.GetText("ProjectsPath");
				const ImVec2& ProjectsPathTextLocalPosition = FGui::GetCursorPosition();

				FGui::Draw(ProjectsPathText);

				FInputTextBox& ProjectsPathInputTextBox = Page.GetInputTextBox("ProjectsPath");
				const ImVec2& ProjectsPathTextSize = FGui::CalculateSize(ProjectsPathText);

				const bool bPathNotValid = !IsValidPath(ProjectsPathInputTextBox.Buffer.get());

				if (bPathNotValid)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
				}
				else
				{
					FSettings::GetInstance().ProjectsFolderPath = ProjectsPathInputTextBox.Buffer.get();
				}

				FGui::SetCursorPosition(ProjectsPathTextLocalPosition);
				FGui::MoveCursorX(ProjectsPathTextSize.x + FGui::GetFramePadding().x);
				FGui::MoveCursorY(ProjectsPathTextSize.y / 2 - FGui::CalculateSize(ProjectsPathInputTextBox).y / 2);
				FGui::Draw(ProjectsPathInputTextBox);

				if (bPathNotValid)
				{
					ImGui::PopStyleColor();
				}

				FGui::SetCursorPositionX(TopOffset.x);
				const FText& LanguageText = Page.GetText("Language");
				const ImVec2& LanguageTextSize = FGui::CalculateSize(LanguageText);
				const ImVec2& LanguageTextLocalPosition = FGui::GetCursorPosition();
				FGui::Draw(LanguageText);
				FGui::SetCursorPosition(LanguageTextLocalPosition);
				FGui::MoveCursorX(LanguageTextSize.x + FGui::GetFramePadding().x);

				static std::vector<const char*> LanguageValuesList =
				{
					TRANSLATE("English"),
					TRANSLATE("Russian")
				};

				ImGui::PushFont(FGui::GetFont(19));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.19921875f, 0.19921875f, 0.19921875f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.14921875f, 0.14921875f, 0.14921875f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.29921875f, 0.29921875f, 0.29921875f, 1.f));
				int& LanguageIndex = FSettings::GetInstance().LanguageIndex;
				int i = 0;
				const ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
				if (ImGui::TreeNodeEx(LanguageValuesList[LanguageIndex], TreeNodeFlags))
				{
					for (const char* Value : LanguageValuesList)
					{
						const ImGuiTreeNodeFlags OverrideFlags = TreeNodeFlags
							| (LanguageIndex == i ? ImGuiTreeNodeFlags_Selected : 0)
							| ImGuiTreeNodeFlags_Leaf;
						ImGui::TreeNodeEx(Value, OverrideFlags, Value);
						if (ImGui::IsItemClicked())
						{
							LanguageIndex = i;
						}
						ImGui::TreePop();
						++i;
					}
					ImGui::TreePop();
				}
				ImGui::PopStyleColor(3);
				ImGui::PopFont();
			}
			FGui::DrawTopPanel(Window);
		}
		FGui::EndWindow();
	}

	void FMainWindow::SetupMainPage()
	{
		FWindow& MainWindow = FGui::GetWindow("Main");
		FPage MainPage;
		FImageButton LogoImage;
		LogoImage.HoveredCursor = ImGuiMouseCursor_Hand;
		LogoImage.UniqueID = "##LogoImageButton";
		LogoImage.Texture = FGui::GetTexture("Logo512");
		LogoImage.Size = ImVec2((float)LogoImage.Texture.Width, (float)LogoImage.Texture.Height) / 7;
		LogoImage.ImageColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		LogoImage.HoveredImageColor = LogoImage.ImageColor;
		LogoImage.ButtonColor = ImVec4();
		LogoImage.ActiveButtonColor = LogoImage.ButtonColor;
		LogoImage.HoveredButtonColor = LogoImage.ButtonColor;
		LogoImage.OnClickEvent = []()
		{
			ImGui::GetPlatformIO().Platform_OpenInShellFn(ImGui::GetCurrentContext(),
				"https://github.com/Pipkaqa/Bloodshot");
		};

		FButton GithubLinkButton;
		GithubLinkButton.HoveredCursor = ImGuiMouseCursor_Hand;
		GithubLinkButton.Text = "Bloodshot Engine";
		GithubLinkButton.Font = FGui::GetFont(25);
		GithubLinkButton.TextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		GithubLinkButton.HoveredTextColor = GithubLinkButton.TextColor;
		GithubLinkButton.Color = ImVec4(0.f, 0.f, 0.f, 0.f);
		GithubLinkButton.ActiveColor = GithubLinkButton.ActiveColor;
		GithubLinkButton.OnClickEvent = LogoImage.OnClickEvent;
		MainPage.AddImageButton("Logo", std::move(LogoImage));
		MainPage.AddButton("GithubLink", std::move(GithubLinkButton));

		FButton ProjectsButton;
		ProjectsButton.HoveredCursor = ImGuiMouseCursor_Hand;
		ProjectsButton.Text = TRANSLATE("Projects");
		ProjectsButton.Font = GithubLinkButton.Font;
		ProjectsButton.TextColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		ProjectsButton.HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		ProjectsButton.Color = ImVec4();
		ProjectsButton.ActiveColor = ImVec4();
		ProjectsButton.HoveredColor = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		ProjectsButton.OnClickEvent = [this]()
		{
			CurrentTab = "Projects";
		};
		MainPage.AddButton("Projects", std::move(ProjectsButton));

		FLine HorizontalMainSeparator;
		HorizontalMainSeparator.Color = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		HorizontalMainSeparator.Thickness = 0.45f;
		MainPage.AddLine("HorizontalMainSeparator", std::move(HorizontalMainSeparator));

		FLine VerticalMainSeparator;
		VerticalMainSeparator.Color = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		VerticalMainSeparator.Thickness = 0.95f;
		MainPage.AddLine("VerticalMainSeparator", std::move(VerticalMainSeparator));

		//FButton ChangelogButton;
		//ChangelogButton.HoveredCursor = ImGuiMouseCursor_Hand;
		//ChangelogButton.Text = "Changelog";
		//ChangelogButton.Font = GithubLinkButton.Font;
		//ChangelogButton.TextColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		//ChangelogButton.HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		//ChangelogButton.Color = ImVec4();
		//ChangelogButton.ActiveColor = ImVec4();
		//ChangelogButton.HoveredColor = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		//ChangelogButton.OnClickEvent = [this]()
		//{
		//	CurrentTab = "Changelog";
		//};
		//MainPage.AddButton("Changelog", std::move(ChangelogButton));

		FButton AddProjectButton;
		AddProjectButton.HoveredCursor = ImGuiMouseCursor_Hand;
		AddProjectButton.Text = TRANSLATE("Add");
		AddProjectButton.Font = FGui::GetFont(21);
		AddProjectButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		AddProjectButton.HoveredTextColor = AddProjectButton.TextColor;
		AddProjectButton.Color = (ImVec4)ImColor(239, 216, 0);
		AddProjectButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		AddProjectButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		AddProjectButton.OnClickEvent = []() { FAddProjectWindow::Open(); };
		MainPage.AddButton("AddProject", std::move(AddProjectButton));

		FButton NewProjectButton;
		NewProjectButton.HoveredCursor = ImGuiMouseCursor_Hand;
		NewProjectButton.Text = TRANSLATE("New Project");
		NewProjectButton.Font = AddProjectButton.Font;
		NewProjectButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		NewProjectButton.HoveredTextColor = NewProjectButton.TextColor;
		NewProjectButton.Color = (ImVec4)ImColor(239, 216, 0);
		NewProjectButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		NewProjectButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		NewProjectButton.OnClickEvent = []() { FNewProjectWindow::Open(); };
		MainPage.AddButton("NewProject", std::move(NewProjectButton));

		FText ProjectButtonNameTextDummy;
		ProjectButtonNameTextDummy.Font = FGui::GetFont(21);
		MainPage.AddText("_ProjectButtonNameTextDummy", std::move(ProjectButtonNameTextDummy));

		FText ProjectButtonPathTextDummy;
		ProjectButtonPathTextDummy.Font = FGui::GetFont(18);
		MainPage.AddText("_ProjectButtonPathTextDummy", std::move(ProjectButtonPathTextDummy));

		FImageButton SettingsImageButton;
		SettingsImageButton.HoveredCursor = ImGuiMouseCursor_Hand;
		SettingsImageButton.UniqueID = "##SettingsImageButton";
		SettingsImageButton.Texture = FGui::GetTexture("SettingsIcon512");
		SettingsImageButton.Size = ImVec2((float)SettingsImageButton.Texture.Width, (float)SettingsImageButton.Texture.Height) / 24;
		SettingsImageButton.ImageColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		SettingsImageButton.HoveredImageColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		SettingsImageButton.ButtonColor = ImVec4();
		SettingsImageButton.ActiveButtonColor = SettingsImageButton.ButtonColor;
		SettingsImageButton.HoveredButtonColor = SettingsImageButton.ButtonColor;
		SettingsImageButton.OnClickEvent = [this]() { CurrentPage = "Settings"; };

		FButton SettingsButton;
		SettingsButton.HoveredCursor = ImGuiMouseCursor_Hand;
		SettingsButton.Text = TRANSLATE("Settings");
		SettingsButton.Font = GithubLinkButton.Font;
		SettingsButton.TextColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		SettingsButton.HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		SettingsButton.Color = ImVec4();
		SettingsButton.ActiveColor = SettingsButton.Color;
		SettingsButton.HoveredColor = SettingsButton.Color;
		SettingsButton.OnClickEvent = SettingsImageButton.OnClickEvent;
		MainPage.AddImageButton("Settings", std::move(SettingsImageButton));
		MainPage.AddButton("Settings", std::move(SettingsButton));

		MainWindow.SetPage("Main", std::move(MainPage));
	}

	void FMainWindow::SetupSettingsPage()
	{
		FWindow& MainWindow = FGui::GetWindow("Main");
		FPage SettingsPage;

		FButton BackButton;
		BackButton.HoveredCursor = ImGuiMouseCursor_Hand;
		BackButton.Text = "##Back";
		BackButton.Font = FGui::GetFont(25);
		BackButton.TextColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		BackButton.HoveredTextColor = BackButton.TextColor;
		BackButton.Color = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
		BackButton.ActiveColor = BackButton.Color;
		BackButton.HoveredColor = ImVec4(0.15f, 0.15f, 0.15f, 1.f);
		BackButton.OnClickEvent = [this]() { CurrentPage = "Main"; };
		SettingsPage.AddButton("Back", std::move(BackButton));

		FImage BackImage;
		BackImage.HoveredCursor = ImGuiMouseCursor_Hand;
		BackImage.Texture = FGui::GetTexture("BackIcon512");
		BackImage.Size = ImVec2((float)BackImage.Texture.Width, (float)BackImage.Texture.Height) / 60.f;
		BackImage.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		BackImage.HoveredColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		SettingsPage.AddImage("Back", std::move(BackImage));

		FLine BackLine;
		BackLine.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		BackLine.Thickness = 0.45f;
		SettingsPage.AddLine("Back", std::move(BackLine));

		FText BackText;
		BackText.HoveredCursor = ImGuiMouseCursor_Hand;
		BackText.Value = TRANSLATE("Settings");
		BackText.Font = BackButton.Font;
		BackText.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		BackText.HoveredColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		SettingsPage.AddText("Back", std::move(BackText));

		FText ProjectsPathText;
		ProjectsPathText.Value = TRANSLATE_FORMAT("{}: ", "Projects Path");
		ProjectsPathText.Font = FGui::GetFont(21);
		ProjectsPathText.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		ProjectsPathText.HoveredColor = ProjectsPathText.Color;

		FInputTextBox ProjectsPathInputTextBox;
		ProjectsPathInputTextBox.Label = "##ProjectsPath";
		ProjectsPathInputTextBox.Font = FGui::GetFont(21);
		ProjectsPathInputTextBox.Buffer = std::make_shared<char[]>(FSettings::GetProjectsFolderPathSize());
		strcpy(ProjectsPathInputTextBox.Buffer.get(), FSettings::GetProjectsFolderPath().GetData());
		ProjectsPathInputTextBox.BufferSize = FSettings::GetProjectsFolderPathSize();
		ProjectsPathInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		SettingsPage.AddInputTextBox("ProjectsPath", std::move(ProjectsPathInputTextBox));

		FText LanguageText;
		LanguageText.Value = TRANSLATE_FORMAT("{}: ", "Language");
		LanguageText.Font = FGui::GetFont(21);
		LanguageText.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		LanguageText.HoveredColor = LanguageText.Color;
		SettingsPage.AddText("Language", std::move(LanguageText));
		SettingsPage.AddText("ProjectsPath", std::move(ProjectsPathText));

		MainWindow.SetPage("Settings", std::move(SettingsPage));
	}

	void FMainWindow::Setup()
	{
		FWindow MainWindow;
		MainWindow.SetTopPanel(FGui::CreateTopPanel([]() { FGui::Shutdown(); }));
		FGui::AddWindow("Main", std::move(MainWindow));
		SetupMainPage();
		SetupSettingsPage();
	}
}
