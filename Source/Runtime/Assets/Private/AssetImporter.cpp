#include "AssetImporter.h"
#include "AssimpHeader.h"
#include "MeshSource.h"
#include "Profiling/ProfilingMacros.h"
#include "RHI.h"

namespace Bloodshot
{
	namespace Private::AssetImporter
	{
		NODISCARD FORCEINLINE static FTextureSource CreateTexture(const aiTextureType Type,
			const ETextureUnit Unit,
			const aiScene* const Scene,
			const aiMaterial* const Material,
			FStringView InPath,
			const bool bFlipped = false)
		{
			BS_PROFILE_FUNCTION();
			if (Material->GetTextureCount(Type) > 0)
			{
				aiString TexturePath;
				const aiReturn Result = Material->GetTexture(Type,
					0,
					&TexturePath,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr);

				if (Result == AI_SUCCESS)
				{
					FTextureSource TextureSource;
					const char* RawTexturePath = TexturePath.C_Str();
					std::filesystem::path ModelPath = InPath.GetData();
					ModelPath = ModelPath.parent_path();
					std::string AbsolutePath = ModelPath.string() + "/" + RawTexturePath;
					const char* RawAbsolutePath = AbsolutePath.c_str();
					TextureSource.SourcePath = RawAbsolutePath;
					const aiTexture* const EmbeddedTexture = Scene->GetEmbeddedTexture(RawTexturePath);
					TextureSource.Name = EmbeddedTexture
						? EmbeddedTexture->mFilename.C_Str()
						: std::filesystem::path(RawTexturePath).filename().string().c_str();
					TextureSource.TextureUnit = Unit;
					TextureSource.SourceTexture = EmbeddedTexture
						? FRHI::CreateTextureFromMemory(EmbeddedTexture->pcData, EmbeddedTexture->mWidth)
						: FRHI::CreateTexture(RawAbsolutePath, bFlipped);
					return TextureSource;
				}
			}

			return FTextureSource();
		}

		FORCEINLINE static void LoadSourceMesh(const aiScene* const Scene,
			const aiMesh* const SourceMesh,
			TArray<FVertex>& OutVertices,
			TArray<uint32_t>& OutIndices,
			TArray<FSourceMeshDescription>& OutSourceMeshes,
			uint32_t& OutMeshVertexCount,
			uint32_t& OutMeshIndexCount)
		{
			BS_PROFILE_FUNCTION();
			FSourceMeshDescription& OutSourceMesh = OutSourceMeshes.EmplaceBackGetRef();
			OutSourceMesh.StartVertex = OutMeshVertexCount;
			const uint32_t SourceMeshVertexCount = SourceMesh->mNumVertices;
			OutMeshVertexCount += SourceMeshVertexCount;
			OutSourceMesh.StartIndex = OutMeshIndexCount;
			const uint32_t SourceMeshIndexCount = SourceMesh->mNumFaces * 3;
			OutMeshIndexCount += SourceMeshIndexCount;
			OutSourceMesh.IndexCount = SourceMeshIndexCount;

			for (size_t i = 0; i < SourceMeshVertexCount; ++i)
			{
				FVertex& OutVertex = OutVertices.EmplaceBackGetRef();
				const aiVector3D& Position = SourceMesh->mVertices[i];
				OutVertex.Position = glm::vec3(Position.x, Position.y, Position.z);
				if (SourceMesh->HasTextureCoords(0))
				{
					const aiVector3D& TextureCoords = SourceMesh->mTextureCoords[0][i];
					OutVertex.TextureCoords = glm::vec2(TextureCoords.x, TextureCoords.y);
				}
				const aiVector3D& Normals = SourceMesh->mNormals[i];
				OutVertex.Normal = glm::vec3(Normals.x, Normals.y, Normals.z);
				if (SourceMesh->HasTangentsAndBitangents())
				{
					const aiVector3D& Tangents = SourceMesh->mTangents[i];
					OutVertex.Tangent = glm::vec3(Tangents.x, Tangents.y, Tangents.z);
				}
			}

			for (size_t i = 0; i < SourceMesh->mNumFaces; ++i)
			{
				const aiFace& Face = SourceMesh->mFaces[i];
				BS_CHECK(Face.mNumIndices == 3);
				const uint32_t* const Indices = Face.mIndices;
				OutIndices.EmplaceBack(Indices[0]);
				OutIndices.EmplaceBack(Indices[1]);
				OutIndices.EmplaceBack(Indices[2]);
			}

			OutSourceMesh.MaterialIndex = SourceMesh->mMaterialIndex;
		}

		static void LoadSourceMeshesFromNodeRecursiveImpl(const aiScene* const Scene,
			const aiNode* const Node,
			TArray<FVertex>& OutVertices,
			TArray<uint32_t>& OutIndices,
			TArray<FSourceMeshDescription>& OutSourceMeshes,
			uint32_t& OutMeshVertexCount,
			uint32_t& OutMeshIndexCount)
		{
			BS_PROFILE_FUNCTION();
			for (size_t i = 0; i < Node->mNumMeshes; ++i)
			{
				LoadSourceMesh(Scene,
					Scene->mMeshes[Node->mMeshes[i]],
					OutVertices,
					OutIndices,
					OutSourceMeshes,
					OutMeshVertexCount,
					OutMeshIndexCount);
			}

			for (size_t i = 0; i < Node->mNumChildren; ++i)
			{
				LoadSourceMeshesFromNodeRecursiveImpl(Scene,
					Node->mChildren[i],
					OutVertices,
					OutIndices,
					OutSourceMeshes,
					OutMeshVertexCount,
					OutMeshIndexCount);
			}
		}

		static void CalculateVertexIndexCount(const aiScene* const Scene,
			const aiNode* const Node,
			uint32_t& MeshVertexCount,
			uint32_t& MeshIndexCount)
		{
			BS_PROFILE_FUNCTION();
			for (size_t i = 0; i < Node->mNumMeshes; ++i)
			{
				MeshVertexCount += Scene->mMeshes[Node->mMeshes[i]]->mNumVertices;
				MeshIndexCount += Scene->mMeshes[Node->mMeshes[i]]->mNumFaces * 3;
			}

			for (size_t i = 0; i < Node->mNumChildren; ++i)
			{
				CalculateVertexIndexCount(Scene,
					Node->mChildren[i],
					MeshVertexCount,
					MeshIndexCount);
			}
		}

		FORCEINLINE static void LoadSourceMeshesFromNodeRecursive(const aiScene* const Scene,
			const aiNode* const Node,
			TArray<FVertex>& OutVertices,
			TArray<uint32_t>& OutIndices,
			TArray<FSourceMeshDescription>& OutSourceMeshes)
		{
			BS_PROFILE_FUNCTION();
			uint32_t MeshVertexCount = 0;
			uint32_t MeshIndexCount = 0;
			CalculateVertexIndexCount(Scene, Node, MeshVertexCount, MeshIndexCount);
			OutVertices.Reserve(MeshVertexCount);
			OutIndices.Reserve(MeshIndexCount);
			MeshVertexCount = 0;
			MeshIndexCount = 0;
			OutSourceMeshes.Reserve(Scene->mNumMeshes);
			LoadSourceMeshesFromNodeRecursiveImpl(Scene,
				Node,
				OutVertices,
				OutIndices,
				OutSourceMeshes,
				MeshVertexCount,
				MeshIndexCount);
		}

		template<typename PropertyType>
		FORCEINLINE static bool GetMaterialProperty(PropertyType& OutValue,
			const aiMaterial* const Material,
			const char* Key,
			const uint32_t Type,
			const uint32_t Index)
		{
			BS_PROFILE_FUNCTION();
			return Material->Get(Key, Type, Index, OutValue) == AI_SUCCESS;
		}

		FORCEINLINE static void LoadMaterials(const aiScene* const Scene,
			TArray<FMaterialSource>& MaterialSources,
			TArray<FTextureSource>& TextureSources,
			FStringView InPath)
		{
			BS_PROFILE_FUNCTION();
			for (size_t i = 0; i < Scene->mNumMaterials; ++i)
			{
				const aiMaterial* const SourceMeshMaterial = Scene->mMaterials[i];

				FMaterialSource& MaterialSource = MaterialSources.EmplaceBackGetRef();
				MaterialSource.Name = SourceMeshMaterial->GetName().C_Str();

				FMaterialProperties& MaterialProperties = MaterialSource.Properties;
				GetMaterialProperty((aiColor3D&)MaterialProperties.BaseColor, SourceMeshMaterial, AI_MATKEY_BASE_COLOR);
				GetMaterialProperty(MaterialProperties.Metallic, SourceMeshMaterial, AI_MATKEY_METALLIC_FACTOR);
				GetMaterialProperty((aiColor3D&)MaterialProperties.Specular, SourceMeshMaterial, AI_MATKEY_COLOR_SPECULAR);
				GetMaterialProperty(MaterialProperties.Roughness, SourceMeshMaterial, AI_MATKEY_ROUGHNESS_FACTOR);
				GetMaterialProperty(MaterialProperties.Anisotropy, SourceMeshMaterial, AI_MATKEY_ANISOTROPY_FACTOR);
				GetMaterialProperty((aiColor3D&)MaterialProperties.EmissiveColor, SourceMeshMaterial, AI_MATKEY_COLOR_EMISSIVE);
				GetMaterialProperty(MaterialProperties.Opacity, SourceMeshMaterial, AI_MATKEY_OPACITY);
				//GetMaterialProperty(MaterialProperties.Normal, SourceMeshMaterial, );
				//GetMaterialProperty(MaterialProperties.Tangent, SourceMeshMaterial, );
				GetMaterialProperty((aiColor3D&)MaterialProperties.Ambient, SourceMeshMaterial, AI_MATKEY_COLOR_AMBIENT);
				GetMaterialProperty((aiColor3D&)MaterialProperties.Diffuse, SourceMeshMaterial, AI_MATKEY_COLOR_DIFFUSE);

				TextureSources.EmplaceBack(CreateTexture(aiTextureType_BASE_COLOR, ETextureUnit::BaseColor, Scene, SourceMeshMaterial, InPath));
				TextureSources.EmplaceBack(CreateTexture(aiTextureType_DIFFUSE, ETextureUnit::Diffuse, Scene, SourceMeshMaterial, InPath));
				TextureSources.EmplaceBack(CreateTexture(aiTextureType_NORMALS, ETextureUnit::NormalMap, Scene, SourceMeshMaterial, InPath));
				TextureSources.EmplaceBack(CreateTexture(aiTextureType_DIFFUSE_ROUGHNESS, ETextureUnit::Roughness, Scene, SourceMeshMaterial, InPath));
				TextureSources.EmplaceBack(CreateTexture(aiTextureType_SPECULAR, ETextureUnit::Specular, Scene, SourceMeshMaterial, InPath));
				TextureSources.EmplaceBack(CreateTexture(aiTextureType_METALNESS, ETextureUnit::Metallic, Scene, SourceMeshMaterial, InPath));
				TextureSources.EmplaceBack(CreateTexture(aiTextureType_HEIGHT, ETextureUnit::HeightMap, Scene, SourceMeshMaterial, InPath));
			}
		}
	}

	FAssetImporter& FAssetImporter::GetInstance()
	{
		static FAssetImporter Instance;
		return Instance;
	}

	void FAssetImporter::ImportMesh(FMeshSource& OutMeshSource, FStringView InPath)
	{
		BS_PROFILE_FUNCTION();
		Assimp::Importer AssetImporter;

		const aiScene* const Scene = AssetImporter.ReadFile(InPath.GetData(),
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

		if (!Scene)
		{
			BS_LOG(Error, "Error occured while loading mesh from file: {}", AssetImporter.GetErrorString());
			return;
		}

		Private::AssetImporter::LoadSourceMeshesFromNodeRecursive(Scene,
			Scene->mRootNode,
			OutMeshSource.Vertices,
			OutMeshSource.Indices,
			OutMeshSource.SourceMeshes);

		Private::AssetImporter::LoadMaterials(Scene,
			OutMeshSource.MaterialSources,
			OutMeshSource.TextureSources,
			InPath);
	}
}
