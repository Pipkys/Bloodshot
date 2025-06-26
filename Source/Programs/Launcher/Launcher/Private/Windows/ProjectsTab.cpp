#include "Windows/ProjectsTab.h"
#include "Settings.h"

#include <fstream>
#include <regex>

namespace Bloodshot::Launcher
{
	FProjectsTab& FProjectsTab::GetInstance()
	{
		static FProjectsTab Instance;
		return Instance;
	}

	void FProjectsTab::Draw()
	{
		UpdateProjects(FSettings::GetProjectsFolderPath().GetData());

		FWindow& MainWindow = FGui::GetWindow("Main");
		FPage& MainPage = MainWindow.GetPage("Main");

		const ImVec2& BottomOffset = MainPage.BottomOffset;
		const ImVec2& CornerProjectListPosition = FGui::GetCursorPosition();

		const ImVec2& FullTextSize = FGui::CalculateSize(MainPage.GetText("_ProjectButtonNameTextDummy"))
			+ FGui::CalculateSize(MainPage.GetText("_ProjectButtonPathTextDummy"));
		const ImVec2& ProjectButtonSize = ImVec2(FGui::GetWindowSize().x - CornerProjectListPosition.x,
			FullTextSize.y + FGui::GetFramePadding().y * 2.f);

		const FButton& NewProjectButton = MainPage.GetButton("NewProject");
		const ImVec2& NewProjectButtonSize = {130, 32};

		FGui::MoveCursorX(ProjectButtonSize.x - NewProjectButtonSize.x - BottomOffset.x);
		FGui::MoveCursorY(ProjectButtonSize.y / 2.f - NewProjectButtonSize.y / 2.f);

		const ImVec2& NewProjectButtonLocalPosition = FGui::GetCursorPosition();

		FGui::Draw(NewProjectButton, NewProjectButtonSize);

		const FButton& AddProjectButton = MainPage.GetButton("AddProject");
		const ImVec2& AddProjectButtonSize = {55, 32};
		const ImVec2& AddProjectButtonExtendedSize = ImVec2(AddProjectButtonSize.x * 2.f, AddProjectButtonSize.y);

		FGui::SetCursorPosition(NewProjectButtonLocalPosition);
		FGui::MoveCursorX(-AddProjectButtonExtendedSize.x - 4.f);
		FGui::SetCursorPositionY(CornerProjectListPosition.y);
		FGui::MoveCursorY(ProjectButtonSize.y / 2.f - AddProjectButtonExtendedSize.y / 2.f);
		FGui::Draw(AddProjectButton, AddProjectButtonExtendedSize);

		if (!MainPage.ContainsWidgetGroup("Projects"))
		{
			return;
		}
		const FWidgetGroup& ProjectButtonGroup = MainPage.GetWidgetGroup("Projects");
		if (!ProjectButtonGroup.GetSize())
		{
			return;
		}

		FGui::SetCursorPosition(CornerProjectListPosition);
		FGui::MoveCursorY(ProjectButtonSize.y);

		const ImVec2& ProjectsSubWindowSize = ImVec2(ProjectButtonSize.x,
			FGui::GetWindowSize().y - CornerProjectListPosition.y - ProjectButtonSize.y);
		ImGui::BeginChild("ProjectsSubWindow", ProjectsSubWindowSize);
		{
			for (size_t i = 0; i < Projects.GetSize(); ++i)
			{
				const FString& CountString = String::Format("{}", i);

				const FText& NameText = ProjectButtonGroup.GetText(FString("Name").Append(CountString));
				const FText& PathText = ProjectButtonGroup.GetText(FString("Path").Append(CountString));

				const ImVec2& FramePadding = FGui::GetFramePadding();
				const ImVec2& NameTextSize = FGui::CalculateSize(NameText);
				const ImVec2& FullTextSize = NameTextSize + FGui::CalculateSize(PathText);
				const ImVec2& CursorLocalPosition = FGui::GetCursorPosition();

				FGui::Draw(ProjectButtonGroup.GetButton(CountString), ProjectButtonSize);

				const ImVec2& ButtonLocalPosition = ImVec2(CursorLocalPosition.x + 16.f, CursorLocalPosition.y);
				const ImVec2& TextPosition = ImVec2(ButtonLocalPosition.x,
					ButtonLocalPosition.y + ProjectButtonSize.y - FullTextSize.y - FramePadding.y);

				FGui::SetCursorPosition(TextPosition);
				FGui::Draw(NameText);

				FGui::SetCursorPosition(TextPosition);
				FGui::MoveCursorX(1.f);
				FGui::MoveCursorY(NameTextSize.y);
				FGui::Draw(PathText);
				FGui::UpdateState(3);

				FGui::SetCursorPosition(CursorLocalPosition);
				FGui::MoveCursorY(ProjectButtonSize.y);
			}
		}
		ImGui::EndChild();
	}

	void FProjectsTab::UpdateProjects(const std::filesystem::path& TargetFolderPath)
	{
		using namespace std::filesystem;
		FWindow& MainWindow = FGui::GetWindow("Main");
		FPage& MainPage = MainWindow.GetPage("Main");
		if (TargetFolderPath.empty())
		{
			return;
		}
		else if (!exists(TargetFolderPath))
		{
			create_directory(TargetFolderPath);
		}
		Projects.Clear();

		ProjectArchive.ForEach([this](const Shared::FProject& InProject)
		{
			Projects.AddTail(InProject);
		});

		FWidgetGroup ProjectButtonGroup;
		size_t ProjectCount = 0;

		ImFont* NameTextFont = FGui::GetFont(21);
		ImFont* PathTextFont = FGui::GetFont(18);

		const ImVec4& ButtonHoveredColor = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		const ImVec4& TextColor = ImVec4(0.85f, 0.85f, 0.85f, 1.f);
		const ImVec4& HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);

		for (const Shared::FProject& Project : Projects)
		{
			const FString& Name = Project.Name;
			const FString& Directory = Project.Directory;
			const FString& CountString = String::Format("{}", ProjectCount);

			FButton Button;
			Button.HoveredCursor = ImGuiMouseCursor_Hand;
			Button.Text = FString("##Project").Append(Name).Append(CountString);
			Button.Font = NameTextFont;
			Button.HoveredColor = ButtonHoveredColor;
			Button.OnClickEvent = [&Project]()
			{
				IProcess::Create(String::Format("Bloodshot Engine.exe Language:{} Project:{}",
					FSettings::GetLanguageIndex(), Project.Directory));
			};
			ProjectButtonGroup.AddButton(FString(CountString), std::move(Button));

			FText NameText;
			NameText.HoveredCursor = ImGuiMouseCursor_Hand;
			NameText.Value = Name;
			NameText.Font = Button.Font;
			NameText.Color = TextColor;
			NameText.HoveredColor = HoveredTextColor;
			ProjectButtonGroup.AddText(std::move(FString("Name").Append(CountString)), std::move(NameText));

			FText PathText;
			PathText.HoveredCursor = ImGuiMouseCursor_Hand;
			PathText.Value = Directory;
			PathText.Font = PathTextFont;
			PathText.Color = TextColor;
			PathText.HoveredColor = HoveredTextColor;
			ProjectButtonGroup.AddText(std::move(FString("Path").Append(CountString)), std::move(PathText));

			++ProjectCount;
		}
		MainPage.SetWidgetGroup("Projects", std::move(ProjectButtonGroup));
		bNeedReloadProjects = false;
	}
}
