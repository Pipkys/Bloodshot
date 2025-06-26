#include "Windows/OutlinerWindow.h"
#include "EditorConfig.h"
#include "EntitySelectionBuffer.h"
#include "ImGuiHeader.h"
#include "Input.h"
#include "Level.h"
#include "Translator.h"

namespace Bloodshot
{
	void FOutlinerWindow::Input()
	{
		if (FInput::GetKeyDown(EKeyCode::KeyDelete))
		{
			FEntitySelectionBuffer::MoveFromSelectedToRequestedToDelete();
		}
	}

	void FOutlinerWindow::Tick()
	{
	}

	void FOutlinerWindow::Render()
	{
		if (ImGui::Ext::Begin(TRANSLATE("Outliner"), bFocused))
		{
			const ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			TSet<FEntity*>& ParentsOnly = FLevel::GetParentsOnly();
			for (FEntity* const Parent : ParentsOnly)
			{
				RenderEntityTreeNodesRecursive(Parent, TreeNodeFlags);
			}
		}
		ImGui::End();
	}

	void FOutlinerWindow::RenderEntityTreeNodesRecursive(FEntity* const Entity, const int TreeNodeFlags)
	{
		const ImGuiTreeNodeFlags OverrideFlags = TreeNodeFlags
			| (FEntitySelectionBuffer::ContainsSelected(Entity) ? ImGuiTreeNodeFlags_Selected : 0)
			| (!Entity->GetChildCount() ? ImGuiTreeNodeFlags_Leaf : 0);

		const bool bTreeNodeOpened = ImGui::TreeNodeEx(Entity, OverrideFlags, Entity->Name.GetData());

		if (IsInputAllowed())
		{
			if (ImGui::IsItemClicked())
			{
				/*if (FInput::GetKeyDown(EKeyCode::KeyLeftShift))
				{

				}
				else */if (FInput::GetKeyDown(EKeyCode::KeyLeftControl))
				{
					const size_t Index = FEntitySelectionBuffer::FindSelected(Entity);
					if (Index != IReservedValues::NoneIndex)
					{
						FEntitySelectionBuffer::RemoveSelected(Index);
					}
					else
					{
						FEntitySelectionBuffer::AddToSelected(Entity);
					}
				}
				else
				{
					FEntitySelectionBuffer::ClearSelected();
					FEntitySelectionBuffer::AddToSelected(Entity);
				}
			}
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete", "DELETE"))
			{
				FEntitySelectionBuffer::AddToDelete(Entity);
			}
			ImGui::EndPopup();
		}

		if (bTreeNodeOpened)
		{
			for (FEntity* const Child : Entity->GetChildren())
			{
				RenderEntityTreeNodesRecursive(Child, TreeNodeFlags);
			}
			ImGui::TreePop();
		}
	}
}
