#pragma once

#include "Core.h"

#include "EditorWindow.h"

namespace Bloodshot
{
	class FTexture;

	struct FAssetPreviewResources final
	{
		FTexture* Icon;
		glm::vec4 OverlineColor;
	};

	class FContentBrowserWindow final : public IEditorWindow
	{
		friend class FEditor;

		FContentBrowserWindow();
		virtual ~FContentBrowserWindow() override;

		const FAssetPreviewResources FolderAssetPreview;
		const FAssetPreviewResources ShaderAssetPreview;
		const FAssetPreviewResources StaticMeshAssetPreview;
		const FAssetPreviewResources MaterialAssetPreview;
		const FAssetPreviewResources CPPClassAssetPreview;

		

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;
	};
}
