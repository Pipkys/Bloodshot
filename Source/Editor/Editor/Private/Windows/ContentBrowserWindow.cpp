#include "Windows/ContentBrowserWindow.h"
#include "ImGuiHeader.h"
#include "RHI.h"
#include "Translator.h"

namespace Bloodshot
{
	FContentBrowserWindow::FContentBrowserWindow()
		: FolderAssetPreview(FRHI::CreateTexture(BuildResourcePath("Editor/ContentBrowser/Icons/FolderIcon.png")), glm::vec4(0.f))
		, ShaderAssetPreview(FRHI::CreateTexture(BuildResourcePath("Editor/ContentBrowser/Icons/ShaderIcon.png")), glm::vec4(0.f, 0.f, 1.f, 1.f))
		, StaticMeshAssetPreview(FRHI::CreateTexture(BuildResourcePath("Editor/ContentBrowser/Icons/StaticMeshIcon.png")), glm::vec4(0.f, 1.f, 1.f, 1.f))
		, MaterialAssetPreview(FRHI::CreateTexture(BuildResourcePath("Editor/ContentBrowser/Icons/MaterialIcon.png")), glm::vec4(0.f, 1.f, 0.f, 1.f))
		, CPPClassAssetPreview(FRHI::CreateTexture(BuildResourcePath("Editor/ContentBrowser/Icons/CPPClassIcon.png")), glm::vec4(1.f, 1.f, 1.f, 1.f))
	{
	}

	FContentBrowserWindow::~FContentBrowserWindow()
	{
		FRHI::DestroyTexture(FolderAssetPreview.Icon);
		FRHI::DestroyTexture(ShaderAssetPreview.Icon);
		FRHI::DestroyTexture(StaticMeshAssetPreview.Icon);
		FRHI::DestroyTexture(MaterialAssetPreview.Icon);
		FRHI::DestroyTexture(CPPClassAssetPreview.Icon);
	}

	void FContentBrowserWindow::Input()
	{
	}

	void FContentBrowserWindow::Tick()
	{
	}

	void FContentBrowserWindow::Render()
	{
		if (ImGui::Ext::Begin(TRANSLATE("Content Browser"), bFocused))
		{

		}
		ImGui::End();
	}
}
