#pragma once

#include "Core.h"

#include "EditorWindow.h"

namespace Bloodshot
{
	class FWorldSettingsWindow final : public IEditorWindow
	{
		friend class FEditor;

		FWorldSettingsWindow() {}
		virtual ~FWorldSettingsWindow() override {}

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;
	};
}
