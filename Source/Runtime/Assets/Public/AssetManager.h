#pragma once

#include "Core.h"

#include "Asset.h"
#include "AssetImporter.h"
#include "IndexBuffer.h"
#include "MaterialAsset.h"
#include "MaterialProperties.h"
#include "MeshSource.h"
#include "Object/ObjectSharedPtr.h"
#include "RHI.h"
#include "ShaderAsset.h"
#include "StaticMeshAsset.h"
#include "Texture.h"
#include "TextureAsset.h"
#include "Vertex.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferElement.h"
#include "VertexBufferLayout.h"

namespace Bloodshot
{
	class FAssetManager final
	{
		friend class Private::Launch::FEngineEditorContext;

	public:
		NODISCARD static FAssetManager& GetInstance();

		NODISCARD FORCEINLINE static FVertexBufferLayout GetDefaultVertexBufferLayout()
		{
			return
			{
				{"in_Position", EShaderDataType::Float3},
				{"in_TextureCoords", EShaderDataType::Float2},
				{"in_Normal", EShaderDataType::Float3},
				{"in_Tangent", EShaderDataType::Float3}
			};
		}

		NODISCARD FORCEINLINE static bool IsNameOccupied(FStringView InName)
		{
			return GetInstance().Assets.Contains(InName);
		}

		template<typename AssetType>
		NODISCARD FORCEINLINE static TAssetSharedPtr<AssetType> GetAsset(FStringView InName)
		{
			return reinterpret_cast<TAssetSharedPtr<AssetType>&>(GetInstance().Assets[InName]->Data);
		}

		template<typename AssetType>
		NODISCARD FORCEINLINE static TAssetSharedPtr<AssetType> GetInternalAsset(FStringView InName)
		{
			return reinterpret_cast<TAssetSharedPtr<AssetType>&>(GetInstance().InternalAssets[InName]->Data);
		}

		NODISCARD FORCEINLINE static TAssetSharedPtr<FShaderAsset> GetDefaultShaderAsset()
		{
			return GetInternalAsset<FShaderAsset>("DefaultShader");
		}

		NODISCARD FORCEINLINE static TAssetSharedPtr<FMaterialAsset> GetDefaultMaterialAsset()
		{
			return GetInternalAsset<FMaterialAsset>("DefaultMaterial");
		}

		static TAssetSharedPtr<FTextureAsset> CreateTextureAsset(FStringView InName,
			FStringView InPath,
			FStringView InSourceFile,
			const ETextureUnit InUnit,
			FAsset** OutAsset = nullptr)
		{
			BS_CHECK(!IsNameOccupied(InName));
			FAssetManager& Instance = GetInstance();

			FAsset* const TextureAssetWrapper = CreateAssetWrapper();
			Instance.Assets.Emplace(InName, TextureAssetWrapper);
			if (OutAsset)
			{
				*OutAsset = TextureAssetWrapper;
			}

			TAssetSharedPtr<FTextureAsset> TextureAsset = NewObject<FTextureAsset>();
			TextureAssetWrapper->Name = InName;
			TextureAssetWrapper->SourceFiles.EmplaceBack(InSourceFile);
			TextureAssetWrapper->Path = InPath;
			TextureAssetWrapper->Data = reinterpret_cast<TAssetSharedPtr<IObject>&>(TextureAsset);
			TextureAssetWrapper->Type = FTextureAsset::StaticClass()->GetName();
			TextureAsset->Unit = InUnit;
			return TextureAsset;
		}

		static TAssetSharedPtr<FTextureAsset> LoadTextureAsset(FStringView InName,
			FStringView InPath,
			FStringView InSourceFile,
			const ETextureUnit InUnit,
			FAsset** OutAsset = nullptr)
		{
			TAssetSharedPtr<FTextureAsset> TextureAsset = CreateTextureAsset(InName, InPath, InSourceFile, InUnit, OutAsset);
			TextureAsset->SourceTexture = FRHI::CreateTexture(InSourceFile);
			return TextureAsset;
		}

		static TAssetSharedPtr<FShaderAsset> CreateShaderAsset(FStringView InName,
			FStringView InVertexShaderSourceFile,
			FStringView InFragmentShaderSourceFile,
			FStringView InPath,
			FAsset** OutAsset = nullptr)
		{
			BS_CHECK(!IsNameOccupied(InName));
			FAssetManager& Instance = GetInstance();

			FAsset* const ShaderAssetWrapper = CreateAssetWrapper();
			if (OutAsset)
			{
				*OutAsset = ShaderAssetWrapper;
			}
			Instance.Assets.Emplace(InName, ShaderAssetWrapper);
			TAssetSharedPtr<FShaderAsset> ShaderAsset = NewObject<FShaderAsset>();
			ShaderAssetWrapper->Name = InName;
			ShaderAssetWrapper->SourceFiles.EmplaceBack(InVertexShaderSourceFile);
			ShaderAssetWrapper->SourceFiles.EmplaceBack(InFragmentShaderSourceFile);
			ShaderAssetWrapper->Path = InPath;
			ShaderAssetWrapper->Data = reinterpret_cast<TAssetSharedPtr<IObject>&>(ShaderAsset);
			ShaderAssetWrapper->Type = FMaterialAsset::StaticClass()->GetName();

			ShaderAsset->SourceShader = FRHI::CreateShaderFromFile(InName, InVertexShaderSourceFile, InFragmentShaderSourceFile);
			return ShaderAsset;
		}

		static TAssetSharedPtr<FMaterialAsset> CreateMaterialAsset(FStringView InName,
			FStringView InPath,
			FStringView InSourceFile,
			FAsset** OutAsset = nullptr)
		{
			BS_CHECK(!IsNameOccupied(InName));
			FAssetManager& Instance = GetInstance();

			FAsset* const MaterialAssetWrapper = CreateAssetWrapper();
			if (OutAsset)
			{
				*OutAsset = MaterialAssetWrapper;
			}
			Instance.Assets.Emplace(InName, MaterialAssetWrapper);
			TAssetSharedPtr<FMaterialAsset> MaterialAsset = NewObject<FMaterialAsset>();
			MaterialAssetWrapper->Name = InName;
			MaterialAssetWrapper->SourceFiles.EmplaceBack(InSourceFile);
			MaterialAssetWrapper->Path = InPath;
			MaterialAssetWrapper->Data = reinterpret_cast<TAssetSharedPtr<IObject>&>(MaterialAsset);
			MaterialAssetWrapper->Type = FMaterialAsset::StaticClass()->GetName();

			return MaterialAsset;
		}

		static TAssetSharedPtr<FStaticMeshAsset> CreateStaticMeshAsset(FStringView InName,
			FStringView InPath,
			FStringView InSourceFile,
			FAsset** OutAsset = nullptr)
		{
			BS_CHECK(!IsNameOccupied(InName));
			FAssetManager& Instance = GetInstance();

			FAsset* const StaticMeshAssetWrapper = CreateAssetWrapper();
			Instance.Assets.Emplace(InName, StaticMeshAssetWrapper);
			if (OutAsset)
			{
				*OutAsset = StaticMeshAssetWrapper;
			}

			TAssetSharedPtr<FStaticMeshAsset> StaticMeshAsset = NewObject<FStaticMeshAsset>();
			StaticMeshAssetWrapper->Name = InName;
			StaticMeshAssetWrapper->SourceFiles.EmplaceBack(InSourceFile);
			StaticMeshAssetWrapper->Path = InPath;
			StaticMeshAssetWrapper->Data = reinterpret_cast<TAssetSharedPtr<IObject>&>(StaticMeshAsset);
			StaticMeshAssetWrapper->Type = FStaticMeshAsset::StaticClass()->GetName();
			return StaticMeshAsset;
		}

		static TAssetSharedPtr<FStaticMeshAsset> LoadStaticMeshAsset(FStringView InName,
			FStringView InPath,
			FStringView InSourceFile,
			FAsset** OutAsset = nullptr)
		{
			TAssetSharedPtr<FStaticMeshAsset> StaticMeshAsset = CreateStaticMeshAsset(InName, InPath, InSourceFile, OutAsset);

			FMeshSource MeshSource;
			FAssetImporter::ImportMesh(MeshSource, InSourceFile);
			StaticMeshAsset->Vertices = std::move(MeshSource.Vertices);
			StaticMeshAsset->Indices = std::move(MeshSource.Indices);
			StaticMeshAsset->SourceMeshes = std::move(MeshSource.SourceMeshes);
			StaticMeshAsset->VertexArray = FRHI::CreateVertexArray();
			FVertexBuffer* const VertexBuffer = FRHI::CreateVertexBuffer(StaticMeshAsset->Vertices, std::move(GetDefaultVertexBufferLayout()));
			StaticMeshAsset->VertexArray->AddVertexBuffer(VertexBuffer);
			FIndexBuffer* const IndexBuffer = FRHI::CreateIndexBuffer(StaticMeshAsset->Indices);
			StaticMeshAsset->VertexArray->SetIndexBuffer(IndexBuffer);
			size_t TexturesOffset = 0;
			for (size_t i = 0; i < MeshSource.MaterialSources.GetSize(); ++i)
			{
				const FMaterialSource& MaterialSource = MeshSource.MaterialSources[i];
				const FString& MaterialName = MaterialSource.Name;
				const FMaterialProperties& MaterialProperties = MaterialSource.Properties;
				TAssetSharedPtr<FMaterialAsset> MaterialAsset = CreateMaterialAsset(MaterialName, InPath, InSourceFile);
				MaterialAsset->ShaderAsset = std::move(GetDefaultShaderAsset());
				MaterialAsset->Properties = MaterialProperties;
				for (size_t i = 0; i < 7; ++i)
				{
					const FTextureSource& TextureSource = MeshSource.TextureSources[TexturesOffset + i];
					const FString& TextureName = TextureSource.Name;
					const FString& TexturePath = TextureSource.SourcePath;
					FTexture* const SourceTexture = TextureSource.SourceTexture;
					if (SourceTexture)
					{
						TAssetSharedPtr<FTextureAsset> TextureAsset = CreateTextureAsset(TextureName, InPath, TexturePath, TextureSource.TextureUnit);
						TextureAsset->SourceTexture = SourceTexture;
						MaterialAsset->TextureAssets.EmplaceBack(std::move(TextureAsset));
					}
				}
				StaticMeshAsset->MaterialAssets.EmplaceBack(std::move(MaterialAsset));
				TexturesOffset += 7;
			}
			return StaticMeshAsset;
		}

		FORCEINLINE static void DestroyAsset(FStringView InName)
		{
			FAssetManager& Instance = GetInstance();
			BS_CHECK(IsNameOccupied(InName));
			FAsset* const Asset = Instance.Assets[InName];
			DeleteObject(Asset->Data.GetObject());
			Asset->Data.MarkObjectAsInvalid();
			Asset->~FAsset();
			Instance.AssetAllocator.Deallocate(Asset);
		}

	private:
		FORCEINLINE static void StaticConstructor()
		{
			FAssetManager& Instance = GetInstance();

			FAsset* const DefaultShaderAssetWrapper = CreateAssetWrapper();
			Instance.InternalAssets.Emplace("DefaultShader", DefaultShaderAssetWrapper);
			TAssetSharedPtr<FShaderAsset> DefaultShaderAsset = NewObject<FShaderAsset>();
			DefaultShaderAssetWrapper->Data = reinterpret_cast<TAssetSharedPtr<IObject>&>(DefaultShaderAsset);
			DefaultShaderAsset->SourceShader = FRHI::CreateShaderFromFile("DefaultShader", BuildResourcePath("Engine/Shaders/Default.vert"), BuildResourcePath("Engine/Shaders/Default.frag"));

			FAsset* const DefaultMaterialAssetWrapper = CreateAssetWrapper();
			Instance.InternalAssets.Emplace("DefaultMaterial", DefaultMaterialAssetWrapper);
			TAssetSharedPtr<FMaterialAsset> DefaultMaterialAsset = NewObject<FMaterialAsset>();
			DefaultMaterialAssetWrapper->Data = reinterpret_cast<TAssetSharedPtr<IObject>&>(DefaultMaterialAsset);
			DefaultMaterialAsset->ShaderAsset = std::move(DefaultShaderAsset);

			if (FTexture* const DefaultTexture = FRHI::CreateTexture(BuildResourcePath("Engine/Materials/DefaultTexture.jpg")))
			{
				TAssetSharedPtr<FTextureAsset> TextureAsset = CreateTextureAsset("Default", BuildResourcePath("Engine/Materials/DefaultTexture.jpg"), BuildResourcePath("Engine/Materials/DefaultTexture.jpg"), ETextureUnit::Diffuse);
				TextureAsset->SourceTexture = DefaultTexture;
				DefaultMaterialAsset->TextureAssets.EmplaceBack(std::move(TextureAsset));
			}

			FAsset* const BoxStaticMeshAssetWrapper = CreateAssetWrapper();
			Instance.InternalAssets.Emplace("Box", BoxStaticMeshAssetWrapper);
			TAssetSharedPtr<FStaticMeshAsset> BoxStaticMeshAsset = NewObject<FStaticMeshAsset>();
			BoxStaticMeshAssetWrapper->Data = reinterpret_cast<TAssetSharedPtr<IObject>&>(BoxStaticMeshAsset);

			BoxStaticMeshAsset->Vertices =
			{
				FVertex{{-1, 1, -1}},
				FVertex{{1, 1, -1}},
				FVertex{{1, -1, -1}},
				FVertex{{-1, -1, -1}},
				FVertex{{-1, 1, 1}},
				FVertex{{1, 1, 1}},
				FVertex{{1, -1, 1}},
				FVertex{{-1, -1, 1}}
			};

			BoxStaticMeshAsset->Indices =
			{
				0, 1, 2,
				0, 2, 3,
				2, 1, 5,
				2, 5, 6,
				3, 2, 6,
				3, 6, 7,
				0, 3, 7,
				0, 7, 4,
				1, 0, 4,
				1, 4, 5,
				6, 5, 4,
				6, 4, 7
			};

			BoxStaticMeshAsset->VertexArray = FRHI::CreateVertexArray();
			FVertexBuffer* const VertexBuffer = FRHI::CreateVertexBuffer(BoxStaticMeshAsset->Vertices, std::move(GetDefaultVertexBufferLayout()));
			BoxStaticMeshAsset->VertexArray->AddVertexBuffer(VertexBuffer);
			FIndexBuffer* const IndexBuffer = FRHI::CreateIndexBuffer(BoxStaticMeshAsset->Indices);
			BoxStaticMeshAsset->VertexArray->SetIndexBuffer(IndexBuffer);
			BoxStaticMeshAsset->MaterialAssets.EmplaceBack(std::move(DefaultMaterialAsset));
		}

		FORCEINLINE static void StaticDestructor()
		{
			FAssetManager& Instance = GetInstance();

			for (TPair<FStringView, FAsset*>& AssetPair : Instance.Assets)
			{
				FAsset* const Asset = AssetPair.Second;
				DeleteObject(Asset->Data.GetObject());
				Asset->Data.MarkObjectAsInvalid();
				Asset->~FAsset();
			}
			Instance.Assets.Dispose();

			for (TPair<FStringView, FAsset*>& InternalAssetPair : Instance.InternalAssets)
			{
				FAsset* const Asset = InternalAssetPair.Second;
				DeleteObject(Asset->Data.GetObject());
				Asset->Data.MarkObjectAsInvalid();
				Asset->~FAsset();
			}
			Instance.InternalAssets.Dispose();
			Instance.AssetAllocator.Dispose();
		}

		template<typename AssetType>
		using TAssetAllocator = TPoolAllocator<512>::ForElementType<AssetType>;

		TAssetAllocator<FAsset> AssetAllocator;
		TMap<FStringView, FAsset*> Assets;
		TMap<FStringView, FAsset*> InternalAssets;

		NODISCARD FORCEINLINE static FAsset* CreateAssetWrapper()
		{
			void* const AssetWrapperMemory = GetInstance().AssetAllocator.Allocate();
			return new(AssetWrapperMemory) FAsset;
		}
	};
}
