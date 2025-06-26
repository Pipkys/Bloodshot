#pragma once

#include "Core.h"

#include "EditorWindow.h"

namespace Bloodshot
{
	class FResourceUsageWindow final : public IEditorWindow
	{
		friend class FEditor;

		FResourceUsageWindow() {}
		virtual ~FResourceUsageWindow() override {}

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;
	};
}
