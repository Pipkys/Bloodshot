#include "Windows/NewProjectWindow.h"
#include "GUI.h"
#include "IsValidPath.h"
#include "Launcher.h"
#include "Project.h"
#include "Settings.h"
#include "Translator.h"
#include "Windows/ProjectsTab.h"

#include <fstream>
#include <regex>

namespace Bloodshot::Launcher
{
	FNewProjectWindow& FNewProjectWindow::GetInstance()
	{
		static FNewProjectWindow Instance;
		return Instance;
	}

	void FNewProjectWindow::Draw()
	{
		if (bOpened)
		{
			FGui::BeginWindow("New Project", 0, ImVec2(512, 256));
			{
				const FWindow& Window = FGui::GetWindow("NewProject");
				const FPage& Page = Window.GetPage("Main");
				const ImVec2& WindowSize = FGui::GetWindowSize();
				const ImVec2& TopOffset = Page.TopOffset;

				const ImVec2& DecorSize = ImVec2(WindowSize.x, 35.f);

				FGui::SetCursorPosition(0, TopOffset.y);
				FGui::Draw(Page.GetButton("Decor"), DecorSize);
				FGui::SetCursorPosition(TopOffset);
				FGui::MoveCursorY(5.f);
				FGui::Draw(Page.GetText("NewProject"));

				bool bValidInput = true;

				const FInputTextBox& ProjectNameInputTextBox = Page.GetInputTextBox("ProjectName");
				const ImVec2& ProjectNameInputTextBoxSize = FGui::CalculateSize(ProjectNameInputTextBox);
				{
					const bool bPathNotValid = !IsValidPath(ProjectNameInputTextBox.Buffer.get());

					if (bPathNotValid)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
					}

					FGui::SetCursorPosition(TopOffset.x, ProjectNameInputTextBoxSize.y + TopOffset.y + DecorSize.y / 2.f);
					FGui::Draw(ProjectNameInputTextBox);

					if (bPathNotValid)
					{
						ImGui::PopStyleColor();
					}
					bValidInput = bValidInput && !bPathNotValid;
				}

				const FInputTextBox& ProjectPathInputTextBox = Page.GetInputTextBox("ProjectPath");
				{
					const bool bPathNotValid = !IsValidPath(ProjectPathInputTextBox.Buffer.get());

					if (bPathNotValid)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
					}

					FGui::SetCursorPositionX(TopOffset.x);
					FGui::Draw(ProjectPathInputTextBox);

					if (bPathNotValid)
					{
						ImGui::PopStyleColor();
					}
					bValidInput = bValidInput && !bPathNotValid;
				}

				const FButton& CreateButton = Page.GetButton("Create");
				const ImVec2& CreateButtonSize = FGui::CalculateSize(CreateButton);
				const ImVec2& BottomOffset = Page.BottomOffset;

				if (CreateButton.State.IsClicked() && bValidInput)
				{
					if (NewProject(ProjectNameInputTextBox.Buffer.get(), ProjectPathInputTextBox.Buffer.get()))
					{
						FProjectsTab::GetInstance().bNeedReloadProjects = true;
						Close();
					}
				}

				FGui::SetCursorPosition(FGui::GetWindowSize() - CreateButtonSize - BottomOffset);
				FGui::Draw(CreateButton);
				FGui::UpdateState();
				FGui::DrawTopPanel(Window);
			}
			FGui::EndWindow();
		}
	}

	void FNewProjectWindow::Setup()
	{
		FWindow NewProjectWindow;
		NewProjectWindow.SetTopPanel(FGui::CreateTopPanel([this]() { Close(); }));

		FPage MainPage;
		FText NewProjectText;
		NewProjectText.Value = TRANSLATE("New Project");
		NewProjectText.Font = FGui::GetFont(24);
		NewProjectText.Color = ImVec4(1.f, 1.f, 1.f, 1.f);
		NewProjectText.HoveredColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		MainPage.AddText("NewProject", std::move(NewProjectText));

		FButton BackButton;
		BackButton.Text = "##Decor";
		BackButton.Font = FGui::GetFont(24);
		BackButton.Color = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
		BackButton.ActiveColor = BackButton.Color;
		BackButton.HoveredColor = BackButton.Color;
		MainPage.AddButton("Decor", std::move(BackButton));

		FInputTextBox ProjectNameInputTextBox;
		ProjectNameInputTextBox.Label = "##ProjectName";
		ProjectNameInputTextBox.Hint = TRANSLATE("Project Name");
		ProjectNameInputTextBox.Font = FGui::GetFont(21);
		ProjectNameInputTextBox.Buffer = std::make_shared<char[]>(1024);
		strcpy(ProjectNameInputTextBox.Buffer.get(), "New Project");
		ProjectNameInputTextBox.BufferSize = 1024;
		ProjectNameInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		MainPage.AddInputTextBox("ProjectName", std::move(ProjectNameInputTextBox));

		FButton CreateButton;
		CreateButton.HoveredCursor = ImGuiMouseCursor_Hand;
		CreateButton.Text = TRANSLATE("Create");
		CreateButton.Font = FGui::GetFont(21);
		CreateButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		CreateButton.HoveredTextColor = CreateButton.TextColor;
		CreateButton.Color = (ImVec4)ImColor(239, 216, 0);
		CreateButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		CreateButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		MainPage.AddButton("Create", std::move(CreateButton));

		FInputTextBox ProjectPathInputTextBox;
		ProjectPathInputTextBox.Label = "##ProjectPath";
		ProjectPathInputTextBox.Hint = TRANSLATE("Project Path");
		ProjectPathInputTextBox.Font = FGui::GetFont(21);
		ProjectPathInputTextBox.Buffer = std::make_shared<char[]>(1024);
		strcpy(ProjectPathInputTextBox.Buffer.get(), FSettings::GetProjectsFolderPath().GetData());
		ProjectPathInputTextBox.BufferSize = 1024;
		ProjectPathInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		MainPage.AddInputTextBox("ProjectPath", std::move(ProjectPathInputTextBox));

		NewProjectWindow.SetPage("Main", std::move(MainPage));
		FGui::AddWindow("NewProject", std::move(NewProjectWindow));
	}

	bool FNewProjectWindow::NewProject(const FString& Name, const std::filesystem::path& TargetFolderPath)
	{
		using namespace std::filesystem;
		if (!exists(TargetFolderPath))
		{
			create_directories(TargetFolderPath);
		}

		Shared::FProject Project;
		Project.Name = Name;
		Project.Directory = FString(TargetFolderPath.string().c_str()) + "/" + Name;
		Project.ContentDirectory = Project.Directory + "/Content";
		Project.EditorStartupScene = Project.Directory + "/Content/Scenes/Default.bsscene";
		Project.GameDefaultScene = Project.EditorStartupScene;

		if (FProjectsTab::GetInstance().ProjectArchive.Contains(Project))
		{
			return false;
		}

		create_directories((Project.Directory + "/Content/Scenes").GetData());

		std::ifstream InputStream("../Resources/Launcher/Generate/Project.txt");
		FString StreamContent = (std::stringstream() << InputStream.rdbuf()).str().c_str();
		InputStream.close();

		StreamContent = std::regex_replace(StreamContent.GetData(),
			std::regex("PLACEHOLDER_PROJECT_NAME"), Project.Name.GetData()).c_str();

		FString BloodshotMainDir = std::filesystem::current_path().parent_path().string().c_str();

		String::Replace(BloodshotMainDir, "\\", "/");
		StreamContent = std::regex_replace(StreamContent.GetData(),
			std::regex("PLACEHOLDER_BLOODSHOT_DIR"), BloodshotMainDir.GetData()).c_str();

		std::ofstream OutputStream((Project.Directory + "/CMakeLists.txt").GetData());
		OutputStream << StreamContent.GetData();
		OutputStream.close();

		std::filesystem::create_directories((Project.Directory +
			String::Format("/Source/{}/Private", Project.Name)).GetData());
		std::filesystem::create_directories((Project.Directory +
			String::Format("/Source/{}/Public", Project.Name)).GetData());

		InputStream.open("../Resources/Launcher/Generate/Module.txt");
		OutputStream.open((Project.Directory +
			String::Format("/Source/{}/CMakeLists.txt", Project.Name)).GetData());
		StreamContent = (std::stringstream() << InputStream.rdbuf()).str().c_str();
		InputStream.close();
		StreamContent = std::regex_replace(StreamContent.GetData(),
			std::regex("PLACEHOLDER_PROJECT_NAME"), Project.Name.GetData()).c_str();
		OutputStream << StreamContent.GetData();
		OutputStream.close();

		OutputStream.open((Project.Directory +
			String::Format("/Source/{}/Public/{}.h", Project.Name, Project.Name)).GetData());
		OutputStream << "#pragma once\n\n";
		OutputStream << "#include \"Core.h\"\n";
		OutputStream.close();
		OutputStream.open((Project.Directory +
			String::Format("/Source/{}/Private/{}.cpp", Project.Name, Project.Name)).GetData());
		OutputStream << String::Format("#include \"{}.h\"", Project.Name).GetData();
		OutputStream.close();

		Project.SaveToFile((Project.Directory
			+ "/"
			+ Name
			+ ".bsproject").GetData());

		FProjectsTab::GetInstance().ProjectArchive.Add(Project);
		return true;
	}

	void FNewProjectWindow::ResetInputTextBoxes()
	{
		const FWindow& Window = FGui::GetWindow("NewProject");
		const FPage& Page = Window.GetPage("Main");

		const FInputTextBox& ProjectNameInputTextBox = Page.GetInputTextBox("ProjectName");
		const FInputTextBox& ProjectPathInputTextBox = Page.GetInputTextBox("ProjectPath");

		strcpy(ProjectNameInputTextBox.Buffer.get(), "New Project");
		strcpy(ProjectPathInputTextBox.Buffer.get(), FSettings::GetProjectsFolderPath().GetData());
	}
}
