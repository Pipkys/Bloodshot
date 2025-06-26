#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) // Variable '(Parent, bActive)' is uninitialized

#include "Core.h"

#include "Component.h"

namespace Bloodshot
{
	class FTransformComponent;

	BSCLASS();
	class FEntity final : public IObject
	{
		friend class FLevel;

		GENERATED_BODY();

	public:
		virtual ~FEntity() override {}

		BSPROPERTY();
		FString Name;

		BSPROPERTY();
		bool bActive;

		NODISCARD FORCEINLINE TArray<IComponent*>& GetComponents()
		{
			return Components;
		}

		NODISCARD FORCEINLINE const TArray<IComponent*>& GetComponents() const
		{
			return Components;
		}

		NODISCARD FORCEINLINE FTransformComponent* GetTransformComponent()
		{
			return reinterpret_cast<FTransformComponent*>(Components[0]);
		}

		NODISCARD FORCEINLINE const FTransformComponent* GetTransformComponent() const
		{
			return reinterpret_cast<const FTransformComponent*>(Components[0]);
		}

		NODISCARD FORCEINLINE FEntity* GetParent()
		{
			return Parent;
		}

		NODISCARD FORCEINLINE const FEntity* GetParent() const
		{
			return Parent;
		}

		NODISCARD FORCEINLINE TArray<FEntity*>& GetChildren()
		{
			return Children;
		}

		NODISCARD FORCEINLINE const TArray<FEntity*>& GetChildren() const
		{
			return Children;
		}

		NODISCARD FORCEINLINE size_t GetChildCount() const
		{
			return Children.GetSize();
		}

		FORCEINLINE void SetParent(FEntity* const InParent)
		{
			Parent = InParent;
		}

		FORCEINLINE void AddChild(FEntity* const InChild)
		{
			BS_CHECK(!Children.Contains(InChild));
			InChild->Parent = this;
			Children.EmplaceBack(InChild);
		}
		
		FORCEINLINE void RemoveChild(FEntity* const InChild)
		{
			BS_CHECK(Children.Contains(InChild));
			InChild->Parent = nullptr;
			Children.Remove(InChild);
		}

		template<IsComponent T, typename... ArgTypes>
		T* AddComponent(ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();
			if (T* ExistingComponent; Components.FindByClass<T>(&ExistingComponent))
			{
				BS_LOG(Trace, "Trying to add existing component to entity");
				return ExistingComponent;
			}
			IComponent* const Component = NewObject<T>(std::forward<ArgTypes>(Args)...);
			BS_LOG(Trace, "Creating component of type: {}...", Component->GetClass()->GetName());
			Component->Owner = this;
			if (FEngineState::IsSimulating())
			{
				Component->BeginPlay();
			}
			Components.EmplaceBack(Component);
			return reinterpret_cast<T*>(Component);
		}

		template<IsComponent T>
		void RemoveComponent()
		{
			BS_PROFILE_FUNCTION();
			T* ExistingComponent;
			size_t Index;
			if (!Components.FindByClass<T>(&ExistingComponent, &Index))
			{
				BS_LOG(Trace, "Trying to remove not existing component from entity");
				return;
			}
			DestroyComponent(ExistingComponent);
			Components.RemoveAtSwap(Index);
		}

		FORCEINLINE void RemoveAllComponents()
		{
			for (size_t i = 1; i < Components.GetSize(); ++i)
			{
				DestroyComponent(Components[i]);
			}
			Components.Resize(1);
		}

		template<IsComponent T>
		NODISCARD T* GetComponent() const
		{
			BS_PROFILE_FUNCTION();
			T* ExistingComponent = nullptr;
			if (!Components.FindByClass<T>(&ExistingComponent))
			{
				BS_LOG(Trace, "Trying to get not existing component from entity");
			}
			return ExistingComponent;
		}

		template<IsComponent T>
		NODISCARD bool HasComponent() const
		{
			BS_PROFILE_FUNCTION();
			return Components.FindByClass<T>();
		}

		void Destroy();

	private:
		TArray<IComponent*> Components;
		FEntity* Parent;
		TArray<FEntity*> Children;

		FORCEINLINE void DestroyComponent(IComponent* const Component) const
		{
			BS_LOG(Trace, "Destroying component of type: {}...", Component->GetClass()->GetName());
			if (FEngineState::IsSimulating())
			{
				Component->EndPlay();
			}
			DeleteObject(Component->GetObject());
		}

		FORCEINLINE void PrivateRemoveAllComponents()
		{
			for (IComponent* const Component : Components)
			{
				DestroyComponent(Component);
			}
			Components.Clear();
		}

		FORCEINLINE void BeginPlay()
		{
			for (size_t i = 1; i < Components.GetSize(); ++i)
			{
				Components[i]->BeginPlay();
			}
		}

		FORCEINLINE void EndPlay()
		{
			for (size_t i = 1; i < Components.GetSize(); ++i)
			{
				Components[i]->EndPlay();
			}
		}
	};
}

#pragma warning(pop)
