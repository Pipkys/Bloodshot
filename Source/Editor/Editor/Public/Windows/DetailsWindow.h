#pragma once

#include "Core.h"

#include "EditorWindow.h"

namespace Bloodshot
{
	class FTexture;

	class FDetailsWindow final : public IEditorWindow
	{
		friend class FEditor;

		FDetailsWindow();
		virtual ~FDetailsWindow() override;

		FTexture* const ResetToDefaultImage;

		TMap<FString, TMap<FStringView, TArray<FProperty*>>> MappedPropertyBuffer;
		TMap<FStringView, TPair<size_t, FStringView>> ComponentsMetaData;
		TArray<void*> TempDataBuffer1;
		TArray<void*> TempDataBuffer2;
		TArray<void*> TempDataBuffer3;

		virtual void Input() override;
		virtual void Tick() override;
		virtual void Render() override;

		bool RenderVector3SliderShared(FStringView InLabel, TArray<glm::vec3*>& InValues, const glm::vec3& InDefaultValue, const float InColumnWidth = 100.f);
		bool RenderVector3Slider(FStringView InLabel, glm::vec3& InValue, const glm::vec3& InDefaultValue, const float InColumnWidth = 100.f);
	};
}
