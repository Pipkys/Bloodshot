#pragma once

#include "Core.h"

#include "EditorWindow.h"

namespace Bloodshot
{
	class FPlaceObjectsWindow final : public IEditorWindow
	{
		friend class FEditor;

		FPlaceObjectsWindow() {}
		virtual ~FPlaceObjectsWindow() override {}

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;
	};
}
