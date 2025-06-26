#pragma once

#include "Core.h"

#include "Component.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Entity.h"
#include "System.h"

namespace Bloodshot
{
	template<IsComponent T>
	using TComponentIterator = Private::Object::TObjectIterator<T>;

	class FEntityTree final
	{
	public:
		NODISCARD FORCEINLINE size_t Add(FEntity* const InParent)
		{
			return Parents.Emplace(InParent);
		}

		FORCEINLINE void Remove(FEntity* const InParent)
		{
			Parents.Remove(InParent);
		}

		NODISCARD FORCEINLINE bool Contains(FEntity* const InParent) const
		{
			return Parents.Contains(InParent);
		}

	private:
		TSet<FEntity*> Parents;
	};

	BSCLASS();
	class FLevel final : public IObject
	{
		friend class FWorld;

		GENERATED_BODY();

	private:
		NODISCARD static FLevel* Self();

	public:
		FORCEINLINE FLevel(FStringView InName)
			: Name(InName)
		{
		}

		NODISCARD FORCEINLINE static FCameraComponent* GetMainCamera()
		{
			return Self()->MainCameraComponent;
		}

		NODISCARD FORCEINLINE static TSet<FEntity*>& GetEntities()
		{
			return Self()->Entities;
		}

		NODISCARD FORCEINLINE static TSet<FEntity*>& GetParentsOnly()
		{
#ifdef BS_WITH_EDITOR
			return Self()->ParentsOnly;
#else
			return *(TSet<FEntity*>*)nullptr;
#endif
		}

		FORCEINLINE static void SetMainCamera(FCameraComponent* const CameraComponent)
		{
			Self()->MainCameraComponent = CameraComponent;
		}

		FORCEINLINE static void SetMainCamera(FEntity* const Camera)
		{
			Self()->MainCameraComponent = Camera->GetComponent<FCameraComponent>();
		}

		FORCEINLINE static FEntity* Instantiate(FEntity* const InParent = nullptr,
			FStringView InName = "Entity",
			const bool bInActive = true)
		{
			BS_PROFILE_FUNCTION();
			FEntity* const Entity = NewObject<FEntity>();
			FLevel* const Self = FLevel::Self();
			InParent
				? InParent->AddChild(Entity)
				: (void)Self->ParentsOnly.Emplace(Entity);
			Entity->Name = InName;
			Entity->bActive = bInActive;
			Entity->AddComponent<FTransformComponent>();
			Self->Entities.Emplace(Entity);
			return Entity;
		}

		NODISCARD FORCEINLINE static TArray<FEntity*> Instantiate(const size_t Count,
			FEntity* const InParent = nullptr,
			FStringView InName = "Entity",
			const bool bInActive = true)
		{
			BS_PROFILE_FUNCTION();
			TArray<FEntity*> Result;
			Result.Reserve(Count);
			for (size_t i = 0; i < Count; ++i)
			{
				Result.EmplaceBack(Instantiate(InParent, InName, bInActive));
			}
			return Result;
		}

		FORCEINLINE static void Instantiate(TArray<FEntity*>& OutResult, const size_t Count,
			FEntity* const InParent = nullptr,
			FStringView InName = "Entity",
			const bool bInActive = true)
		{
			BS_PROFILE_FUNCTION();
			OutResult.Resize(Count);
			for (FEntity*& Entity : OutResult)
			{
				Entity = Instantiate(InParent, InName, bInActive);
			}
		}

		FORCEINLINE static void Destroy(FEntity* const Entity)
		{
			BS_PROFILE_FUNCTION();
			for (FEntity* const Child : Entity->Children)
			{
				Child->Parent = nullptr;
				Destroy(Child);
			}
			FLevel* const Self = FLevel::Self();
			Entity->Parent
				? Entity->Parent->RemoveChild(Entity)
				: Self->ParentsOnly.Remove(Entity);
			Entity->PrivateRemoveAllComponents();
			DeleteObject(Entity->GetObject());
			Self->Entities.Remove(Entity);
		}

		FORCEINLINE static void Destroy(TArray<FEntity*>& InOutEntities)
		{
			BS_PROFILE_FUNCTION();
			for (FEntity* const Entity : InOutEntities)
			{
				Destroy(Entity);
			}
			InOutEntities.Clear();
		}

		template<IsComponent T>
		NODISCARD FORCEINLINE static TComponentIterator<T> CreateComponentIterator()
		{
			return Private::Object::FObjectCore::CreateObjectIterator<T>();
		}

		template<IsSystem T, typename... ArgTypes>
		FORCEINLINE static T* AddSystem(ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();
			if (T* System; ContainsSystem<T>(System))
			{
				BS_LOG(Error, "Trying to add already existing system");
				return System;
			}

			T* const System = NewObject<T>(std::forward<ArgTypes>(Args)...);
			Self()->Systems.EmplaceBack(System);
			return System;
		}

		template<IsSystem T>
		FORCEINLINE static void RemoveSystem()
		{
			BS_PROFILE_FUNCTION();
			T* System;
			size_t Index;

			if (!ContainsSystem<T>(System, &Index))
			{
				BS_LOG(Error, "Trying to remove not existing System");
				return;
			}

			DeleteObject(System->GetObject());
			Self()->Systems[Index] = nullptr;
		}

		FORCEINLINE static void RemoveAllSystems()
		{
			BS_PROFILE_FUNCTION();
			TArray<ISystem*>& Systems = Self()->Systems;
			for (ISystem* const System : Systems)
			{
				DeleteObject(System->GetObject());
			}
			Systems.Clear();
		}

		template<IsSystem T>
		NODISCARD FORCEINLINE static T* GetSystem()
		{
			BS_PROFILE_FUNCTION();

			if (T* System; !ContainsSystem<T>(System))
			{
				BS_LOG(Error, "Trying to get not existing system");
				return nullptr;
			}
			else
			{
				return System;
			}
		}

		template<IsSystem T>
		FORCEINLINE static void EnableSystem()
		{
			BS_PROFILE_FUNCTION();

			if (T* System; !ContainsSystem(System))
			{
				BS_LOG(Error, "Trying to enable not existing system");
				return;
			}
			else
			{
				System->bEnabled = true;
			}
		}

		template<IsSystem T>
		FORCEINLINE static void DisableSystem()
		{
			BS_PROFILE_FUNCTION();

			if (T* System; !ContainsSystem(System))
			{
				BS_LOG(Error, "Trying to disable not existing system");
				return;
			}
			else
			{
				System->bEnabled = false;
			}
		}

	private:
		FString Name;
		TSet<FEntity*> Entities;
#ifdef BS_WITH_EDITOR
		TSet<FEntity*> ParentsOnly;
#endif
		TArray<ISystem*> Systems;
		FCameraComponent* MainCameraComponent = nullptr;

		template<IsSystem T>
		NODISCARD FORCEINLINE static bool ContainsSystem(T* OutSystem, size_t* const OutIndex = nullptr)
		{
			BS_PROFILE_FUNCTION();
			return Self()->Systems.FindByClass<T>(&OutSystem, OutIndex);
		}

		FORCEINLINE void BeginPlay()
		{
			for (FEntity* const Entity : Entities)
			{
				Entity->BeginPlay();
			}
		}

		FORCEINLINE void EndPlay()
		{
			for (FEntity* const Entity : Entities)
			{
				Entity->EndPlay();
			}
		}

		FORCEINLINE void Tick(float DeltaTime)
		{
			// BSTODO: Fixed time stamp
			{
				for (ISystem* const System : Systems)
				{
					if (System)
					{
						System->FixedTick();
					}
				}
			}

			for (ISystem* const System : Systems)
			{
				if (System)
				{
					System->Tick(DeltaTime);
				}
			}

			for (ISystem* const System : Systems)
			{
				if (System)
				{
					System->LateTick(DeltaTime);
				}
			}
		}
	};
}
