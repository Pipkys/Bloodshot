#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FEntity;

	class FEntitySelectionBuffer final
	{
	public:
		NODISCARD static FEntitySelectionBuffer& GetInstance();

		NODISCARD FORCEINLINE static TArray<FEntity*>& GetSelected()
		{
			return GetInstance().Selected;
		}

		NODISCARD FORCEINLINE static TArray<FEntity*>& GetRequestedToDelete()
		{
			return GetInstance().RequestedToDelete;
		}

		FORCEINLINE static void AddToSelected(FEntity* const Entity)
		{
			GetInstance().Selected.EmplaceBack(Entity);
		}

		FORCEINLINE static void AddToSelected(TArray<FEntity*>& Entities)
		{
			GetInstance().Selected.Append(Entities);
		}

		FORCEINLINE static void AddToDelete(FEntity* const Entity)
		{
			GetInstance().RequestedToDelete.EmplaceBack(Entity);
		}

		FORCEINLINE static void AddToDelete(TArray<FEntity*>& Entities)
		{
			GetInstance().RequestedToDelete.Append(Entities);
		}

		FORCEINLINE static void ClearSelected()
		{
			GetInstance().Selected.Clear();
		}

		FORCEINLINE static void ClearToDelete()
		{
			GetInstance().RequestedToDelete.Clear();
		}

		FORCEINLINE static void MoveFromRequestedToDeleteToSelected()
		{
			FEntitySelectionBuffer& Instance = GetInstance();
			TArray<FEntity*>& RequestedToDelete = Instance.RequestedToDelete;
			Instance.Selected.Append(RequestedToDelete);
			RequestedToDelete.Clear();
		}

		FORCEINLINE static void MoveFromSelectedToRequestedToDelete()
		{
			FEntitySelectionBuffer& Instance = GetInstance();
			TArray<FEntity*>& Selected = Instance.Selected;
			Instance.RequestedToDelete.Append(Selected);
			Selected.Clear();
		}

		FORCEINLINE static void RemoveSelected(const size_t Index)
		{
			GetInstance().Selected.RemoveAt(Index);
		}

		FORCEINLINE static void RemoveSelected(FEntity* const Entity)
		{
			GetInstance().Selected.Remove(Entity);
		}

		NODISCARD FORCEINLINE static size_t FindSelected(FEntity* const Entity)
		{
			return GetInstance().Selected.Find(Entity);
		}

		NODISCARD FORCEINLINE static bool ContainsSelected(FEntity* const Entity)
		{
			return GetInstance().Selected.Contains(Entity);
		}

	private:
		TArray<FEntity*> Selected;
		TArray<FEntity*> RequestedToDelete;
	};
}
