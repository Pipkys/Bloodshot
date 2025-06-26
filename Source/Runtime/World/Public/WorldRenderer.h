#pragma once

#include "Camera.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core.h"
#include "Entity.h"
#include "Level.h"
#include "RHI.h"
#include "UniformBuffer.h"

namespace Bloodshot
{
	class FWorldRenderer final
	{
	public:
		NODISCARD static FWorldRenderer& GetInstance();

		FORCEINLINE static void StaticConstructor()
		{
			GetInstance().MatricesBuffer = FRHI::CreateUniformBuffer(sizeof(glm::mat4) * 3, 0);
		}

		FORCEINLINE static void StaticDestructor()
		{
			FRHI::DestroyUniformBuffer(GetInstance().MatricesBuffer);
		}

		FORCEINLINE static void Render(const FCamera* const Camera, const glm::vec3& CameraPosition)
		{
			const glm::mat4& ViewMatrix = Camera->GetViewMatrix();
			const glm::mat4& ProjectionMatrix = Camera->GetProjectionMatrix();
			const glm::mat4& ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			FUniformBuffer* const MatricesBuffer = GetInstance().MatricesBuffer;

			FLightComponent* LightComponent = nullptr;
			auto LightComponentIterator = FLevel::CreateComponentIterator<FLightComponent>();
			for (; LightComponentIterator; ++LightComponentIterator)
			{
				LightComponent = &*LightComponentIterator;
			}

			using FStaticMeshComponentIterator = TComponentIterator<FStaticMeshComponent>;
			FStaticMeshComponentIterator StaticMeshComponentIterator = FLevel::CreateComponentIterator<FStaticMeshComponent>();
			for (; StaticMeshComponentIterator; ++StaticMeshComponentIterator)
			{
				if (!StaticMeshComponentIterator->bActive)
				{
					continue;
				}

				const FTransformComponent* const TransformComponent = StaticMeshComponentIterator->GetOwner()->GetComponent<FTransformComponent>();
				const glm::mat4& ModelMatrix = TransformComponent->CalculateModelMatrix();

				MatricesBuffer->SetData(glm::value_ptr(ViewProjectionMatrix * ModelMatrix), sizeof(glm::mat4));
				MatricesBuffer->SetData(glm::value_ptr(ModelMatrix), sizeof(glm::mat4), sizeof(glm::mat4));
				MatricesBuffer->SetData(glm::value_ptr(ViewProjectionMatrix), sizeof(glm::mat4), sizeof(glm::mat4) * 2);

				TAssetSharedPtr<FStaticMeshAsset>& StaticMeshAssetPtr = StaticMeshComponentIterator->StaticMeshAsset;
				if (!StaticMeshAssetPtr.IsObjectValid())
				{
					continue;
				}

				FStaticMeshAsset* const StaticMeshAsset = StaticMeshAssetPtr.GetObject();
				TArray<FVertex>& Vertices = StaticMeshAsset->Vertices;
				TArray<uint32_t>& Indices = StaticMeshAsset->Indices;
				TArray<FSourceMeshDescription>& SourceMeshes = StaticMeshAsset->SourceMeshes;
				TArray<TAssetSharedPtr<FMaterialAsset>>& MaterialAssets = StaticMeshAsset->MaterialAssets;
				FVertexArray* const VertexArray = StaticMeshAsset->VertexArray;
				if (!VertexArray)
				{
					continue;
				}

				const size_t SourceMeshCount = SourceMeshes.GetSize();
				if (SourceMeshCount)
				{
					for (const FSourceMeshDescription& SourceMesh : SourceMeshes)
					{
						TAssetSharedPtr<FMaterialAsset>& MaterialAssetPtr = MaterialAssets[SourceMesh.MaterialIndex];
						const bool bMaterialAssetValid = MaterialAssetPtr.IsObjectValid();
						if (bMaterialAssetValid)
						{
							MaterialAssetPtr->BindShader();
							MaterialAssetPtr->ShaderAsset->SetUniformInt("BaseColorTexture", 0);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("DiffuseTexture", 1);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("NormalMapTexture", 2);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("RoughnessTexture", 3);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("SpecularTexture", 4);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("MetallicTexture", 5);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("HeighMapTexture", 6);
							MaterialAssetPtr->BindTextures();
							MaterialAssetPtr->ShaderAsset->SetUniformVec3("CameraPosition", CameraPosition);
							if (LightComponent)
							{
								MaterialAssetPtr->ShaderAsset->SetUniformVec3("LightPosition", LightComponent->GetOwner()->GetTransformComponent()->GetPosition());
								MaterialAssetPtr->ShaderAsset->SetUniformVec4("LightColor", LightComponent->Color);
							}
							else
							{
								MaterialAssetPtr->ShaderAsset->SetUniformVec3("LightPosition", glm::vec3(0.f));
								MaterialAssetPtr->ShaderAsset->SetUniformVec4("LightColor", glm::vec4(0.f, 0.f, 0.f, 1.f));
							}
						}
						FRHI::DrawSourceMesh(VertexArray, SourceMesh);
						if (bMaterialAssetValid)
						{
							MaterialAssetPtr->Unbind();
						}
					}
				}
				else
				{
					const bool bHasMaterialAsset = MaterialAssets.GetSize();
					if (bHasMaterialAsset)
					{
						TAssetSharedPtr<FMaterialAsset>& MaterialAssetPtr = MaterialAssets[0];
						const bool bMaterialAssetValid = MaterialAssetPtr.IsObjectValid();
						if (bMaterialAssetValid)
						{
							MaterialAssetPtr->BindShader();
							MaterialAssetPtr->ShaderAsset->SetUniformInt("BaseColorTexture", 0);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("DiffuseTexture", 1);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("NormalMapTexture", 2);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("RoughnessTexture", 3);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("SpecularTexture", 4);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("MetallicTexture", 5);
							MaterialAssetPtr->ShaderAsset->SetUniformInt("HeighMapTexture", 6);
							MaterialAssetPtr->BindTextures();
							MaterialAssetPtr->ShaderAsset->SetUniformVec3("CameraPosition", CameraPosition);
							if (LightComponent)
							{
								MaterialAssetPtr->ShaderAsset->SetUniformVec3("LightPosition", LightComponent->GetOwner()->GetTransformComponent()->GetPosition());
								MaterialAssetPtr->ShaderAsset->SetUniformVec4("LightColor", LightComponent->Color);
							}
							else
							{
								MaterialAssetPtr->ShaderAsset->SetUniformVec3("LightPosition", glm::vec3(0.f));
								MaterialAssetPtr->ShaderAsset->SetUniformVec4("LightColor", glm::vec4(0.f, 0.f, 0.f, 1.f));
							}
						}
						FRHI::DrawTriangles(VertexArray);
						if (bMaterialAssetValid)
						{
							MaterialAssetPtr->Unbind();
						}
					}
					else
					{
						FRHI::DrawTriangles(VertexArray);
					}
				}
			}
		}

		FORCEINLINE static void Render(const FCameraComponent* const CameraComponent)
		{
			Render(CameraComponent->GetNativeCamera(), CameraComponent->GetOwner()->GetTransformComponent()->GetPosition());
		}

		FORCEINLINE static void Render(const FEntity* const Camera)
		{
			Render(Camera->GetComponent<FCameraComponent>());
		}

	private:
		FUniformBuffer* MatricesBuffer;
	};
}
