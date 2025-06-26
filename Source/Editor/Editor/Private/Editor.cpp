#include "Editor.h"
#include "EditorConfig.h"
#include "EntitySelectionBuffer.h"
#include "ImGuiHeader.h"
#include "Level.h"
#include "Misc/Application.h"
#include "OpenGL/OpenGLHeader.h"
#include "RHI.h"
#include "Translator.h"
#include "Windows/ContentBrowserWindow.h"
#include "Windows/DetailsWindow.h"
#include "Windows/OutlinerWindow.h"
#include "Windows/PlaceObjectsWindow.h"
#include "Windows/ResourceUsageWindow.h"
#include "Windows/StatsWindow.h"
#include "Windows/TestsWindow.h"
#include "Windows/ViewportWindow.h"
#include "Windows/WorldSettingsWindow.h"

// Test
#include "AssetManager.h"
#include "Components/StaticMeshComponent.h"
#include "World.h"

THIRD_PARTY_INCLUDES_START
#include <ImGuizmo.h>
THIRD_PARTY_INCLUDES_END

namespace Bloodshot
{
	namespace Private::Editor
	{
		static inline GLFWwindow* Window = nullptr;
	}

	FEditor& FEditor::GetInstance()
	{
		static FEditor Instance;
		return Instance;
	}

	void FEditor::InitGraphics()
	{
		const int GLFWInitResult = glfwInit();
		BS_ASSERT(GLFWInitResult, "FEditor::StaticConstructor: Failed to init GLFW");
		
		GLFWwindow*& Window = Private::Editor::Window;
		Window = FRHI::GetNativeWindow();

		FRHI::SetNativeWindow(Window);
		glfwMakeContextCurrent(Window);

		const int GladLoadGLResult = gladLoadGL();
		BS_ASSERT(GladLoadGLResult, "FEditor::StaticConstructor: Failed to load GLAD");

		ImGui::CreateContext();
		ImGui_ImplOpenGL3_Init("#version 460");
		ImGui_ImplGlfw_InitForOpenGL(Window, true);
	}

	void FEditor::SetupInterface()
	{
		ImVec4* const StyleColors = ImGui::GetStyle().Colors;
		StyleColors[ImGuiCol_Text] = ImColor(237, 237, 237, 255);
		StyleColors[ImGuiCol_TextDisabled] = ImColor(128, 128, 128, 255);
		StyleColors[ImGuiCol_WindowBg] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_ChildBg] = ImColor(255, 0, 255, 0);
		StyleColors[ImGuiCol_PopupBg] = ImColor(45, 45, 45, 240);
		StyleColors[ImGuiCol_Border] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
		StyleColors[ImGuiCol_FrameBg] = ImColor(0, 0, 0, 150);
		StyleColors[ImGuiCol_FrameBgHovered] = ImColor(0, 0, 0, 150);
		StyleColors[ImGuiCol_FrameBgActive] = ImColor(0, 0, 0, 150);
		StyleColors[ImGuiCol_TitleBg] = ImColor(46, 46, 46, 255);
		StyleColors[ImGuiCol_TitleBgActive] = ImColor(56, 56, 56, 255);
		StyleColors[ImGuiCol_TitleBgCollapsed] = ImColor(46, 46, 46, 255);
		StyleColors[ImGuiCol_MenuBarBg] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_ScrollbarBg] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_ScrollbarGrab] = ImColor(107, 107, 107, 255);
		StyleColors[ImGuiCol_ScrollbarGrabHovered] = ImColor(162, 162, 162, 255);
		StyleColors[ImGuiCol_ScrollbarGrabActive] = ImColor(162, 162, 162, 255);
		StyleColors[ImGuiCol_CheckMark] = ImColor(0, 82, 255, 255);
		StyleColors[ImGuiCol_SliderGrab] = ImColor(0, 82, 217, 255);
		StyleColors[ImGuiCol_SliderGrabActive] = ImColor(0, 82, 255, 255);
		StyleColors[ImGuiCol_Button] = ImColor(48, 48, 48, 255);
		StyleColors[ImGuiCol_ButtonHovered] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_ButtonActive] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_Header] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_HeaderHovered] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_HeaderActive] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_Separator] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_SeparatorHovered] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_SeparatorActive] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_ResizeGrip] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_ResizeGripHovered] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_ResizeGripActive] = ImColor(55, 55, 55, 255);
		StyleColors[ImGuiCol_TabHovered] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_Tab] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_TabSelected] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_TabSelectedOverline] = ImColor(0, 82, 255, 255);
		StyleColors[ImGuiCol_TabDimmed] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_TabDimmedSelected] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_TabDimmedSelectedOverline] = ImColor(0, 82, 255, 252);
		StyleColors[ImGuiCol_DockingPreview] = ImColor(0, 95, 255, 78);
		StyleColors[ImGuiCol_DockingEmptyBg] = ImColor(36, 36, 36, 130);
		StyleColors[ImGuiCol_PlotLines] = ImColor(156, 156, 156, 255);
		StyleColors[ImGuiCol_PlotLinesHovered] = ImColor(255, 110, 89, 255);
		StyleColors[ImGuiCol_PlotHistogram] = ImColor(230, 179, 0, 255);
		StyleColors[ImGuiCol_PlotHistogramHovered] = ImColor(255, 153, 0, 255);
		StyleColors[ImGuiCol_TableHeaderBg] = ImColor(48, 48, 51, 255);
		StyleColors[ImGuiCol_TableBorderStrong] = ImColor(79, 79, 89, 255);
		StyleColors[ImGuiCol_TableBorderLight] = ImColor(59, 59, 64, 255);
		StyleColors[ImGuiCol_TableRowBg] = ImColor(0, 0, 0, 0);
		StyleColors[ImGuiCol_TableRowBgAlt] = ImColor(255, 255, 255, 15);
		StyleColors[ImGuiCol_TextLink] = ImColor(66, 150, 250, 255);
		StyleColors[ImGuiCol_TextSelectedBg] = ImColor(3, 78, 247, 89);
		StyleColors[ImGuiCol_DragDropTarget] = ImColor(255, 255, 0, 230);
		StyleColors[ImGuiCol_NavCursor] = ImColor(3, 78, 247, 255);
		StyleColors[ImGuiCol_NavWindowingHighlight] = ImColor(255, 255, 255, 179);
		StyleColors[ImGuiCol_NavWindowingDimBg] = ImColor(204, 204, 204, 51);
		StyleColors[ImGuiCol_ModalWindowDimBg] = ImColor(204, 204, 204, 89);

		ImGuiIO& ImGuiIO = ImGui::GetIO();
		ImGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGuiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGuiIO.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		ImGuiIO.Fonts->AddFontDefault();

		GetInstance().Font = (void*)ImGuiIO.Fonts->AddFontFromFileTTF(BuildResourcePath("Common/Fonts/PT_Sans-Web-Regular.ttf").GetData(), 18, nullptr, ImGuiIO.Fonts->GetGlyphRangesCyrillic());

		// BSTODO: Simplify?
		using Shared::FTranslator;
		using Shared::ELanguage;
		const ELanguage CurrentLanguage = FTranslator::GetCurrentLanguage();
		FTranslator::SetLanguage(ELanguage::Russian);
		FTranslator::AddPhraseToCurrentDictionary("File", UTF8TEXT("Файл"));
		FTranslator::AddPhraseToCurrentDictionary("Open Project", UTF8TEXT("Открыть Проект"));
		FTranslator::AddPhraseToCurrentDictionary("New Level", UTF8TEXT("Новый Уровень"));
		FTranslator::AddPhraseToCurrentDictionary("Save Level", UTF8TEXT("Сохранить Уровень"));
		FTranslator::AddPhraseToCurrentDictionary("Save Level As", UTF8TEXT("Сохранить Уровень Как"));
		FTranslator::AddPhraseToCurrentDictionary("Exit", UTF8TEXT("Выйти"));
		FTranslator::AddPhraseToCurrentDictionary("Window", UTF8TEXT("Окно"));
		FTranslator::AddPhraseToCurrentDictionary("Content Browser", UTF8TEXT("Браузер Контента"));
		FTranslator::AddPhraseToCurrentDictionary("Details", UTF8TEXT("Детали"));
		FTranslator::AddPhraseToCurrentDictionary("Outliner", UTF8TEXT("Обводчик"));
		FTranslator::AddPhraseToCurrentDictionary("Viewport", UTF8TEXT("Окно Просмотра"));
		FTranslator::AddPhraseToCurrentDictionary("Place Objects", UTF8TEXT("Расположить Объекты"));
		FTranslator::AddPhraseToCurrentDictionary("World Settings", UTF8TEXT("Настройки Мира"));
		FTranslator::AddPhraseToCurrentDictionary("Tools", UTF8TEXT("Инструменты"));
		FTranslator::AddPhraseToCurrentDictionary("Stats", UTF8TEXT("Статистика"));
		FTranslator::AddPhraseToCurrentDictionary("Resource Usage", UTF8TEXT("Использование Ресурсов"));
		FTranslator::AddPhraseToCurrentDictionary("Automation Tests", UTF8TEXT("Автоматизированные Тесты"));
		FTranslator::AddPhraseToCurrentDictionary("Lightning", UTF8TEXT("Освещение"));
		FTranslator::AddPhraseToCurrentDictionary("Geometry", UTF8TEXT("Геометрия"));
		FTranslator::SetLanguage(CurrentLanguage);
	}

	void FEditor::CreateWindows()
	{
		TMap<FStringView, IEditorWindow*>& Windows = GetInstance().Windows;

		Windows.Emplace("Content Browser", new FContentBrowserWindow);
		Windows.Emplace("Details", new FDetailsWindow);
		Windows.Emplace("Outliner", new FOutlinerWindow);
		Windows.Emplace("Viewport", new FViewportWindow);
		Windows.Emplace("Place Objects", new FPlaceObjectsWindow);
		Windows.Emplace("World Settings", new FWorldSettingsWindow);

		Windows.Emplace("Stats", new FStatsWindow);
		Windows.Emplace("Resource Usage", new FResourceUsageWindow);
		Windows.Emplace("Automation Tests", new FTestsWindow);
	}

	void FEditor::DeinitGraphics()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwTerminate();
	}

	void FEditor::DestroyWindows()
	{
		TMap<FStringView, IEditorWindow*>& Windows = GetInstance().Windows;
		for (TPair<FStringView, IEditorWindow*>& WindowPair : Windows)
		{
			delete WindowPair.Second;
		}
	}

	static void Test()
	{
		FWorld::AddLevel("Default");
		FWorld::LoadLevel("Default");
		{
			FEntity* const Model = FLevel::Instantiate(nullptr, "Hamster");
			TAssetSharedPtr<FStaticMeshAsset> StaticMeshAsset = FAssetManager::LoadStaticMeshAsset("SM_Model", "Content", BuildResourcePath("Demo/Hamster/Humster-bl.obj"));
			FStaticMeshComponent* const StaticMeshComponent = Model->AddComponent<FStaticMeshComponent>(StaticMeshAsset);
			FTransformComponent* const TransformComponent = Model->GetComponent<FTransformComponent>();
			TransformComponent->SetPosition(glm::vec3(5, -3, -2.16));
			TransformComponent->SetRotation(glm::vec3(90, 120, 0));
			TransformComponent->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
			FLevel::Instantiate(Model, "Test Child");
		}
		{
			FEntity* const Model = FLevel::Instantiate(nullptr, "Minecraft Village");
			TAssetSharedPtr<FStaticMeshAsset> StaticMeshAsset = FAssetManager::LoadStaticMeshAsset("SM_MinecraftVillage", "Content", BuildResourcePath("Demo/Minecraft/MinecraftVillage.obj"));
			TAssetSharedPtr<FTextureAsset> TextureAsset = FAssetManager::LoadTextureAsset("Minecraft", "Content", BuildResourcePath("Demo/Minecraft/MinecraftVillageTexture.png"), ETextureUnit::Diffuse);
			TAssetSharedPtr<FMaterialAsset> MaterialAsset1 = StaticMeshAsset->MaterialAssets[0];
			MaterialAsset1->TextureAssets.EmplaceBack(TextureAsset);
			TAssetSharedPtr<FMaterialAsset> MaterialAsset2 = StaticMeshAsset->MaterialAssets[1];
			MaterialAsset2->TextureAssets.EmplaceBack(TextureAsset);
			FStaticMeshComponent* const StaticMeshComponent = Model->AddComponent<FStaticMeshComponent>(StaticMeshAsset);
			FTransformComponent* const TransformComponent = Model->GetComponent<FTransformComponent>();
			TransformComponent->SetPosition(glm::vec3(5, 0, -10));
			TransformComponent->SetRotation(glm::vec3(90, 180, 0));
		}
		{
			TAssetSharedPtr<FMaterialAsset> SkyboxMaterialAsset = FAssetManager::CreateMaterialAsset("SkyboxMaterial", "Content", "");
			TAssetSharedPtr<FTextureAsset> SkyboxTexture = FAssetManager::LoadTextureAsset("SkyboxTexture", "Content", BuildResourcePath("Demo/Skybox/CubemapSkyboxCrop.png"), ETextureUnit::Diffuse);
			SkyboxMaterialAsset->TextureAssets.EmplaceBack(SkyboxTexture);
			SkyboxMaterialAsset->ShaderAsset = FAssetManager::GetDefaultShaderAsset();

			FEntity* const Model = FLevel::Instantiate(nullptr, "Skybox");
			TAssetSharedPtr<FStaticMeshAsset> StaticMeshAsset = FAssetManager::LoadStaticMeshAsset("SM_Skybox", "Content", BuildResourcePath("Demo/Skybox/Sky.obj"));
			FStaticMeshComponent* const StaticMeshComponent = Model->AddComponent<FStaticMeshComponent>(StaticMeshAsset);
			StaticMeshAsset->MaterialAssets.Clear();
			StaticMeshAsset->MaterialAssets.EmplaceBack(SkyboxMaterialAsset);
			StaticMeshAsset->MaterialAssets.EmplaceBack(std::move(SkyboxMaterialAsset));
			FTransformComponent* const TransformComponent = Model->GetComponent<FTransformComponent>();
			TransformComponent->SetPosition(glm::vec3(0, 0, 0));
			TransformComponent->SetRotation(glm::vec3(90, 180, 0));
			TransformComponent->SetScale(glm::vec3(-500, 500, 500));
		}
	}

	void FEditor::StaticConstructor()
	{
		BS_LOG(Debug, "Creating FEditor...");
		InitGraphics();
		SetupInterface();
		CreateWindows();
		Test();
	}

	void FEditor::StaticDestructor()
	{
		BS_LOG(Debug, "Destroying FEditor...");
		DestroyWindows();
		DeinitGraphics();
	}

	void FEditor::Tick()
	{
		TMap<FStringView, IEditorWindow*>& Windows = GetInstance().Windows;
		for (TPair<FStringView, IEditorWindow*>& WindowPair : Windows)
		{
			IEditorWindow* const Window = WindowPair.Second;
			if (Window->IsOpen() && Window->IsInputAllowed())
			{
				Window->Input();
			}
		}
		for (TPair<FStringView, IEditorWindow*>& WindowPair : Windows)
		{
			IEditorWindow* const Window = WindowPair.Second;
			if (Window->IsOpen())
			{
				Window->Tick();
			}
		}
		TArray<FEntity*>& RequestedToDelete = FEntitySelectionBuffer::GetRequestedToDelete();
		if (RequestedToDelete.GetSize())
		{
			for (FEntity* const EntityToDelete : RequestedToDelete)
			{
				FLevel::Destroy(EntityToDelete);
			}
			RequestedToDelete.Clear();
		}
	}

	void FEditor::RenderGizmos()
	{
	}

	void FEditor::RenderGui()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::PushFont((ImFont*)GetInstance().Font);

		constexpr ImGuiWindowFlags DockspaceWindowFlags = ImGuiWindowFlags_MenuBar
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* const Viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(Viewport->Pos);
		ImGui::SetNextWindowSize(Viewport->Size);
		ImGui::SetNextWindowViewport(Viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Dockspace", nullptr, DockspaceWindowFlags);
		ImGui::PopStyleVar(3);

		const ImGuiID DockspaceID = ImGui::GetID("Dockspace");
		ImGuiStyle& ImGuiStyle = ImGui::GetStyle();
		const float MinWindowSizeX = ImGuiStyle.WindowMinSize.x;
		ImGuiStyle.WindowMinSize.x = 370.f;
		ImGui::DockSpace(DockspaceID, ImVec2(0, 0));
		ImGuiStyle.WindowMinSize.x = MinWindowSizeX;

		TMap<FStringView, IEditorWindow*>& Windows = GetInstance().Windows;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(TRANSLATE("File")))
			{
				if (ImGui::MenuItem(TRANSLATE_FORMAT("{}...", "Open Project").GetData(), "Ctrl+O"))
				{
				}
				ImGui::Separator();
				if (ImGui::MenuItem(TRANSLATE("New Level"), "Ctrl+N"))
				{
				}
				if (ImGui::MenuItem(TRANSLATE("Save Level"), "Ctrl+S"))
				{
				}
				if (ImGui::MenuItem(TRANSLATE_FORMAT("{}...", "Save Level As").GetData(), "Ctrl+Shift+S"))
				{
				}
				ImGui::Separator();
				if (ImGui::MenuItem(TRANSLATE("Exit")))
				{
					FApplication::RequestExit();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(TRANSLATE("Window")))
			{
				IEditorWindow* const ContentBrowserWindow = Windows["Content Browser"];
				ImGui::MenuItem(TRANSLATE("Content Browser"), nullptr, &ContentBrowserWindow->bOpen);
				IEditorWindow* const DetailsWindow = Windows["Details"];
				ImGui::MenuItem(TRANSLATE("Details"), nullptr, &DetailsWindow->bOpen);
				IEditorWindow* const OutlinerWindow = Windows["Outliner"];
				ImGui::MenuItem(TRANSLATE("Outliner"), nullptr, &OutlinerWindow->bOpen);
				IEditorWindow* const ViewportWindow = Windows["Viewport"];
				ImGui::MenuItem(TRANSLATE("Viewport"), nullptr, &ViewportWindow->bOpen);
				IEditorWindow* const PlaceObjectsWindow = Windows["Place Objects"];
				ImGui::MenuItem(TRANSLATE("Place Objects"), nullptr, &PlaceObjectsWindow->bOpen);
				IEditorWindow* const WorldSettingsWindow = Windows["World Settings"];
				ImGui::MenuItem(TRANSLATE("World Settings"), nullptr, &WorldSettingsWindow->bOpen);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(TRANSLATE("Tools")))
			{
				IEditorWindow* const StatsWindow = Windows["Stats"];
				ImGui::MenuItem(TRANSLATE("Stats"), nullptr, &StatsWindow->bOpen);
				IEditorWindow* const ResourceUsageWindow = Windows["Resource Usage"];
				ImGui::MenuItem(TRANSLATE("Resource Usage"), nullptr, &ResourceUsageWindow->bOpen);
				IEditorWindow* const TestsWindow = Windows["Automation Tests"];
				ImGui::MenuItem(TRANSLATE("Automation Tests"), nullptr, &TestsWindow->bOpen);
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, IEditorConfig::EditorWindowMinSize);
		for (TPair<FStringView, IEditorWindow*>& WindowPair : Windows)
		{
			IEditorWindow* const Window = WindowPair.Second;
			if (Window->IsOpen())
			{
				Window->Render();
			}
		}
		ImGui::PopStyleVar();
		ImGui::PopFont();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		GLFWwindow* CurrentContext = FRHI::GetNativeWindow();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		FRHI::SetNativeWindow(CurrentContext);
		glfwMakeContextCurrent(CurrentContext);
	}
}
