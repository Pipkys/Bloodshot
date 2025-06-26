#pragma once

#include "Core.h"

#include "EditorWindow.h"
#include "Project.h"

namespace Bloodshot
{
	class FEditor final
	{
		friend class Private::Launch::FEngineEditorContext;

	public:
		NODISCARD static FEditor& GetInstance();

	private:
		FEditor() {}

		TMap<FStringView, IEditorWindow*> Windows;
		std::function<void()> OnStartSimulate;
		std::function<void()> OnEndSimulate;
		void* Font = nullptr;

		static void InitGraphics();
		static void SetupInterface();
		static void CreateWindows();

		static void DeinitGraphics();
		static void DestroyWindows();

		static void StaticConstructor();
		static void StaticDestructor();

		template<typename EventType>
		FORCEINLINE static void SetOnStartSimulateEvent(EventType&& InEvent)
		{
			GetInstance().OnStartSimulate = std::move(InEvent);
		}

		template<typename EventType>
		FORCEINLINE static void SetOnEndSimulateEvent(EventType&& InEvent)
		{
			GetInstance().OnEndSimulate = std::move(InEvent);
		}

		static void Tick();
		static void RenderGizmos();
		static void RenderGui();
	};
}
