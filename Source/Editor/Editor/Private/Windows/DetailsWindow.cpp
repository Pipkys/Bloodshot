#include "Windows/DetailsWindow.h"
#include "Component.h"
#include "EditorConfig.h"
#include "Entity.h"
#include "EntitySelectionBuffer.h"
#include "ImGuiHeader.h"
#include "Level.h"
#include "Translator.h"
#include "World.h"

namespace Bloodshot
{
	namespace Private::Editor::DetailsWindow
	{
		NODISCARD FORCEINLINE static bool IsPointer(FStringView SubStr)
		{
			return SubStr.Contains("*");
		}

		NODISCARD FORCEINLINE static bool IsTemplate(FStringView SubStr)
		{
			return SubStr.Contains("<") || SubStr.Contains(">");
		}

		NODISCARD FORCEINLINE static bool IsTransform(FStringView SubStr)
		{
			return SubStr.Contains("FTransform") && !IsTemplate(SubStr);
		}

		NODISCARD FORCEINLINE static FString SplitByUpperCase(FStringView Str)
		{
			FString Result;
			Result.Reserve(24);
			for (size_t i = 0; i < Str.GetSize(); ++i)
			{
				if (isupper(Str[i]) && i)
				{
					Result.AppendChars(Str.GetData(), i);
					Result.AppendChar(' ');
					Str = Str.SubStr(i);
					i = 0;
				}
			}
			Result.AppendChars(Str.GetData(), Str.GetSize());
			return Result;
		}
	}

	FDetailsWindow::FDetailsWindow()
		: ResetToDefaultImage(FRHI::CreateTexture(BuildResourcePath("Editor/DetailsWindow/Icons/ResetToDefaultIcon.png")))
	{
	}

	FDetailsWindow::~FDetailsWindow()
	{
		FRHI::DestroyTexture(ResetToDefaultImage);
	}

	void FDetailsWindow::Input()
	{
	}

	void FDetailsWindow::Tick()
	{
	}

	void FDetailsWindow::Render()
	{
		if (ImGui::Ext::Begin(TRANSLATE("Details"), bFocused))
		{
			TArray<FEntity*>& Selected = FEntitySelectionBuffer::GetSelected();
			const size_t SelectedCount = Selected.GetSize();
			if (SelectedCount > 1)
			{
				for (FEntity* const Entity : Selected)
				{
					for (IComponent* const Component : Entity->GetComponents())
					{
						FClass* const Class = Component->GetClass();
						FStringView ComponentName = Class->GetName();
						FString ComponentFullName = Class->GetNamespace();
						ComponentFullName.Append("::");
						ComponentFullName.Append(ComponentName);
						FProperty* const Properties = Class->GetProperties();
						TPair<FString, TMap<FStringView, TArray<FProperty*>>>& ComponentPropertiesPair = MappedPropertyBuffer.FindOrAddGetPair(std::move(ComponentFullName));
						TMap<FStringView, TArray<FProperty*>>& ComponentProperties = ComponentPropertiesPair.Second;
						++ComponentsMetaData.FindOrAdd(ComponentPropertiesPair.First, {0, ComponentName}).First;
						for (size_t i = 0; i < Class->GetPropertyCount(); ++i)
						{
							FProperty* const Property = Properties + i;
							ComponentProperties.FindOrAdd(Property->GetType()).EmplaceBack(Property);
						}
					}
				}

				for (TPair<FString, TMap<FStringView, TArray<FProperty*>>>& MappedPropertiesPair : MappedPropertyBuffer)
				{
					FStringView ComponentFullName = MappedPropertiesPair.First;
					const TPair<size_t, FStringView> ComponentMetaData = ComponentsMetaData[ComponentFullName];
					const size_t ComponentOccurrences = ComponentMetaData.First;
					if (ComponentOccurrences == SelectedCount)
					{
						FStringView ComponentName = ComponentMetaData.Second;
						const FString& PrettyComponentName = Private::Editor::DetailsWindow::SplitByUpperCase(ComponentName.SubStr(1));
						const bool bHeaderOpened = ImGui::CollapsingHeader(PrettyComponentName.GetData());
						ImGui::SetItemTooltip(ComponentFullName.SubStr(2).GetData());
						if (bHeaderOpened)
						{
							TMap<FStringView, TArray<FProperty*>>& SortedProperties = MappedPropertiesPair.Second;
							for (TPair<FStringView, TArray<FProperty*>>& Properties : SortedProperties)
							{
								FStringView PropertyType = Properties.First;
								if (Private::Editor::DetailsWindow::IsTransform(PropertyType))
								{
									for (FProperty* const Property : Properties.Second)
									{
										FTransform* const Transform = Private::Editor::DetailsWindow::IsPointer(PropertyType)
											? *Property->GetValue<FTransform*>()
											: Property->GetValue<FTransform>();
										TempDataBuffer1.EmplaceBack(&const_cast<glm::vec3&>(Transform->GetPosition()));
										TempDataBuffer2.EmplaceBack(&const_cast<glm::vec3&>(Transform->GetRotation()));
										TempDataBuffer3.EmplaceBack(&const_cast<glm::vec3&>(Transform->GetScale()));
									}
									RenderVector3SliderShared("Position", *reinterpret_cast<TArray<glm::vec3*>*>(&TempDataBuffer1), glm::vec3(0.f, 0.f, 0.f));
									if (RenderVector3SliderShared("Rotation", *reinterpret_cast<TArray<glm::vec3*>*>(&TempDataBuffer2), glm::vec3(0.f, 0.f, 0.f)))
									{
										for (size_t i = 0; i < Properties.Second.GetSize(); ++i)
										{
											FProperty* const Property = Properties.Second[i];
											FTransform* const Transform = Private::Editor::DetailsWindow::IsPointer(PropertyType)
												? *Property->GetValue<FTransform*>()
												: Property->GetValue<FTransform>();
											Transform->SetRotation(*(*reinterpret_cast<TArray<glm::vec3*>*>(&TempDataBuffer2))[i]);
										}
									}
									RenderVector3SliderShared("Scale", *reinterpret_cast<TArray<glm::vec3*>*>(&TempDataBuffer3), glm::vec3(1.f, 1.f, 1.f));
								}
								TempDataBuffer1.Clear();
								TempDataBuffer2.Clear();
								TempDataBuffer3.Clear();
							}
						}
					}
				}

				MappedPropertyBuffer.Clear();
				ComponentsMetaData.Clear();

			}
			else if (SelectedCount)
			{
				FEntity* const Entity = Selected[0];
				for (IComponent* const Component : Entity->GetComponents())
				{
					FClass* const Class = Component->GetClass();
					FStringView ComponentName = FStringView(Class->GetName());
					FString ComponentFullName = Class->GetNamespace();
					ComponentFullName.Append("::");
					ComponentFullName.Append(ComponentName);
					const FString& PrettyName = Private::Editor::DetailsWindow::SplitByUpperCase(ComponentName.SubStr(1));
					const bool bHeaderOpened = ImGui::CollapsingHeader(PrettyName.GetData());
					ImGui::SetItemTooltip(FStringView(ComponentFullName).SubStr(2).GetData());
					if (bHeaderOpened)
					{
						FProperty* const Properties = Class->GetProperties();
						for (size_t i = 0; i < Class->GetPropertyCount(); ++i)
						{
							FProperty* const Property = &Properties[i];
							FStringView Type = Property->GetType();
							if (Private::Editor::DetailsWindow::IsTransform(Type))
							{
								FTransform* const Transform = Private::Editor::DetailsWindow::IsPointer(Type)
									? *Property->GetValue<FTransform*>()
									: Property->GetValue<FTransform>();

								RenderVector3Slider("Position", const_cast<glm::vec3&>(Transform->GetPosition()), glm::vec3(0.f, 0.f, 0.f));
								glm::vec3 Rotation = Transform->GetRotation();
								if (RenderVector3Slider("Rotation", Rotation, glm::vec3(0.f, 0.f, 0.f)))
								{
									Transform->SetRotation(Rotation);
								}
								RenderVector3Slider("Scale", const_cast<glm::vec3&>(Transform->GetScale()), glm::vec3(1.f, 1.f, 1.f));
							}
						}
					}
				}
			}
		}
		ImGui::End();
	}

	bool FDetailsWindow::RenderVector3SliderShared(FStringView InLabel, TArray<glm::vec3*>& InValues, const glm::vec3& InDefaultValue, const float InColumnWidth)
	{
		ImGuiIO& IO = ImGui::GetIO();
		ImGuiStyle& Style = ImGui::GetStyle();
		ImFont* const Font = ImGui::GetFont();
		const char* Label = InLabel.GetData();
		bool bValuesIsDefault = true;
		bool bValuesChanged = false;
		for (glm::vec3* const Value : InValues)
		{
			if (*Value != InDefaultValue)
			{
				bValuesIsDefault = false;
			}
		}

		if (ImGui::BeginTable(Label, bValuesIsDefault ? 2 : 3))
		{
			ImGui::PushID(Label);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.f);
			ImGui::TableSetupColumn("");
			!bValuesIsDefault
				? ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoClip)
				: (void)0;

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text(Label);
			ImGui::TableNextColumn();
			ImGui::PushMultiItemsWidths(3, 200.f);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, 0));

			const size_t ValuesCount = InValues.GetSize();
			constexpr float FloatMax = std::numeric_limits<float>::max();
			constexpr float FloatMin = std::numeric_limits<float>::lowest();

			auto RenderDragFloat = [&ValuesCount, &InValues, &bValuesChanged](const size_t VectorElementIndex, const char* Label)
			{
				bool bSameValues = true;
				float TempBuffer = 0.f;
				bool bInInputTriggered;
				char FormatBuffer[64];
				size_t FormattedStrSize;
				float SameValue;
				for (size_t i = 0; i < ValuesCount; ++i)
				{
					for (size_t j = 0; j < ValuesCount - i; ++j)
					{
						if (*(&InValues[i]->x + VectorElementIndex) != *(&InValues[j]->x + VectorElementIndex))
						{
							bSameValues = false;
						}
					}
				}
				if (bSameValues)
				{
					SameValue = *(&InValues[0]->x + VectorElementIndex);
					FormattedStrSize = String::Format(" {} ", FormatBuffer, SameValue);
				}
				else
				{
					memcpy(FormatBuffer, " Multiple Values ", 18);
					FormattedStrSize = 17;
				}
				TempBuffer = bSameValues ? SameValue : TempBuffer;
				const bool bValueChanged = ImGui::Ext::DragFloat(Label,
					&TempBuffer,
					0.1f,
					FloatMin,
					FloatMax,
					FormatBuffer,
					ImGuiSliderFlags_AlwaysClamp,
					bInInputTriggered);
				FormatBuffer[FormattedStrSize - 1] = '\0';
				ImGui::SetItemTooltip(FStringView(FormatBuffer).SubStr(1).GetData());
				if (bValueChanged)
				{
					if (bInInputTriggered || bSameValues)
					{
						for (glm::vec3* const Value : InValues)
						{
							*(&Value->x + VectorElementIndex) = TempBuffer;
						}
					}
					else
					{
						for (glm::vec3* const Value : InValues)
						{
							*(&Value->x + VectorElementIndex) += TempBuffer;
						}
					}
					bValuesChanged = true;
				}
				ImGui::PopItemWidth();
			};

			RenderDragFloat(0, "##X");
			ImGui::SameLine();
			RenderDragFloat(1, "##Y");
			ImGui::SameLine();
			RenderDragFloat(2, "##Z");

			bool bIsDefaultValues = true;
			for (glm::vec3* const Value : InValues)
			{
				if (*Value != InDefaultValue)
				{
					bIsDefaultValues = false;
				}
			}

			if (!bIsDefaultValues)
			{
				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				const float LastItemRectSizeY = ImGui::GetItemRectSize().y;
				const ImVec2 ButtonSize = ImVec2(LastItemRectSizeY, LastItemRectSizeY);
				const ImVec2 DefaultFramePadding = Style.FramePadding;
				Style.FramePadding = ImVec2(0.f, 0.f);
				if (ImGui::Ext::ImageButton(InLabel.GetData(),
					ResetToDefaultImage->GetUniqueID(),
					ButtonSize,
					ImVec2(0.f, 0.f),
					ImVec2(1.f, 1.f),
					ImVec4(0.f, 0.f, 0.f, 0.f),
					ImVec4(0.9f, 0.9f, 0.9f, 1.f),
					ImVec4(1.f, 1.f, 1.f, 1.f),
					ImVec4(0.7f, 0.7f, 0.7f, 1.f)))
				{
					for (glm::vec3* const Value : InValues)
					{
						*Value = InDefaultValue;
					}
					bValuesChanged = true;
				}
				Style.FramePadding = DefaultFramePadding;
				ImGui::PopStyleColor(3);
			}

			ImGui::PopStyleVar();
			ImGui::PopID();
			ImGui::EndTable();
		}

		return bValuesChanged;
	}

	bool FDetailsWindow::RenderVector3Slider(FStringView InLabel, glm::vec3& InValue, const glm::vec3& InDefaultValue, const float InColumnWidth)
	{
		ImGuiIO& IO = ImGui::GetIO();
		ImGuiStyle& Style = ImGui::GetStyle();
		ImFont* const Font = ImGui::GetFont();
		const char* Label = InLabel.GetData();
		const bool bValueIsDefault = InValue == InDefaultValue;
		bool bValueChanged = false;

		if (ImGui::BeginTable(Label, bValueIsDefault ? 2 : 3))
		{
			ImGui::PushID(Label);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.f);
			ImGui::TableSetupColumn("");
			!bValueIsDefault
				? ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoClip)
				: (void)0;

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text(Label);
			ImGui::TableNextColumn();
			ImGui::PushMultiItemsWidths(3, 200.f);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, 0));

			constexpr float FloatMax = std::numeric_limits<float>::max();
			constexpr float FloatMin = std::numeric_limits<float>::lowest();

			auto RenderDragFloat = [&InValue, &bValueChanged](const size_t VectorElementIndex, const char* Label)
			{
				char FormatBuffer[64];
				float* const Element = &InValue.x + VectorElementIndex;
				const size_t FormattedStrSize = String::Format(" {} ", FormatBuffer, *Element);
				if (ImGui::DragFloat(Label,
					Element,
					0.1f,
					FloatMin,
					FloatMax,
					FormatBuffer,
					ImGuiSliderFlags_AlwaysClamp))
				{
					bValueChanged = true;
				}
				FormatBuffer[FormattedStrSize - 1] = '\0';
				ImGui::SetItemTooltip(FStringView(FormatBuffer).SubStr(1).GetData());
				ImGui::PopItemWidth();
			};

			RenderDragFloat(0, "##X");
			ImGui::SameLine();
			RenderDragFloat(1, "##Y");
			ImGui::SameLine();
			RenderDragFloat(2, "##Z");

			if (!bValueIsDefault)
			{
				ImGui::TableNextColumn();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
				const float LastItemRectSizeY = ImGui::GetItemRectSize().y;
				const ImVec2 ButtonSize = ImVec2(LastItemRectSizeY, LastItemRectSizeY);
				const ImVec2 DefaultFramePadding = Style.FramePadding;
				Style.FramePadding = ImVec2(0.f, 0.f);
				if (ImGui::Ext::ImageButton(InLabel.GetData(),
					ResetToDefaultImage->GetUniqueID(),
					ButtonSize,
					ImVec2(0.f, 0.f),
					ImVec2(1.f, 1.f),
					ImVec4(0.f, 0.f, 0.f, 0.f),
					ImVec4(0.9f, 0.9f, 0.9f, 1.f),
					ImVec4(1.f, 1.f, 1.f, 1.f),
					ImVec4(0.7f, 0.7f, 0.7f, 1.f)))
				{
					InValue = InDefaultValue;
					bValueChanged = true;
				}
				Style.FramePadding = DefaultFramePadding;
				ImGui::PopStyleColor(3);
			}

			ImGui::PopStyleVar();
			ImGui::PopID();
			ImGui::EndTable();
		}

		return bValueChanged;
	}
}
