#pragma once

#include "Core.h"

#include "EditorWindow.h"

namespace Bloodshot
{
	class FTexture;

	class FTestsWindow final : public IEditorWindow
	{
		friend class FEditor;

		FTestsWindow();
		virtual ~FTestsWindow() override;

		FTexture* const TestImage;

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;
	};
}
