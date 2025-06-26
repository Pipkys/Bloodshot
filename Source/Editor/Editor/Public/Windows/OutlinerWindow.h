#pragma once

#include "Core.h"

#include "EditorWindow.h"

namespace Bloodshot
{
	class FEntity;

	class FOutlinerWindow final : public IEditorWindow
	{
		friend class FEditor;

		FOutlinerWindow() {}
		virtual ~FOutlinerWindow() override {}

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;

		void RenderEntityTreeNodesRecursive(FEntity* const Entity, const int TreeNodeFlags);
	};
}
