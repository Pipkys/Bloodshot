#pragma once

#include "Core.h"

#include "EditorWindow.h"

namespace Bloodshot
{
	class FStatsWindow final : public IEditorWindow
	{
		friend class FEditor;

		FStatsWindow() {}
		virtual ~FStatsWindow() override {}

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;
	};
}
