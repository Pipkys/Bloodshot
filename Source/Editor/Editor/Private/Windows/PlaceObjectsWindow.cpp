#include "Windows/PlaceObjectsWindow.h"
#include "Asset.h"
#include "AssetManager.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ImGuiHeader.h"
#include "Level.h"
#include "RHI.h"
#include "Translator.h"

namespace Bloodshot
{
	void FPlaceObjectsWindow::Input()
	{
	}

	void FPlaceObjectsWindow::Tick()
	{
	}

	void FPlaceObjectsWindow::Render()
	{
		if (ImGui::Ext::Begin(TRANSLATE("Place Objects"), bFocused))
		{
			if (ImGui::CollapsingHeader(TRANSLATE("Geometry")))
			{
				if (ImGui::Button("Box"))
				{
					FEntity* const Box = FLevel::Instantiate(nullptr, "Box");
					TAssetSharedPtr<FStaticMeshAsset> StaticMeshAsset = FAssetManager::GetInternalAsset<FStaticMeshAsset>("Box");
					Box->AddComponent<FStaticMeshComponent>(StaticMeshAsset);
				}
				if (ImGui::Button("Cone"))
				{

				}
				if (ImGui::Button("Cylinder"))
				{

				}
				if (ImGui::Button("Sphere"))
				{

				}
			}
			if (ImGui::CollapsingHeader(TRANSLATE("Lightning")))
			{
				if (ImGui::Button("Point Light"))
				{
					FEntity* const PointLight = FLevel::Instantiate(nullptr, "Point Light");
					PointLight->AddComponent<FLightComponent>();
				}
				if (ImGui::Button("Spot Light"))
				{

				}
				if (ImGui::Button("Directional Light"))
				{

				}
			}
		}
		ImGui::End();
	}
}
