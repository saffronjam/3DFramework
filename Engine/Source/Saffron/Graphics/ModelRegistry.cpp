#include "SaffronPCH.h"

#define NOMINMAX

#include "Saffron/Graphics/ModelRegistry.h"

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "assimp/pbrmaterial.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Graphics/Material.h"
#include "Saffron/Rendering/VertexStorage.h"

struct LogStream : Assimp::LogStream
{
	static void Initialize()
	{
		if (Assimp::DefaultLogger::isNullLogger())
		{
			Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
			Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
		}
	}

	void write(const char* message) override
	{
		Se::Log::Info("Assimp error: {0}", message);
	}
};

namespace Se
{
const std::filesystem::path ModelRegistry::BasePath("Assets/Models/");

const uint ModelRegistry::DefaultImportFlags = aiProcess_CalcTangentSpace | // Create binormals/tangents just in case
	aiProcess_Triangulate | // Make sure we're triangles
	aiProcess_SortByPType | // Split meshes by primitive type
	aiProcess_GenNormals | // Make sure we have legit normals
	aiProcess_GenUVCoords | // Convert UVs if required 
	aiProcess_OptimizeMeshes | // Batch draws where possible
	aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure; // Validation

ModelRegistry::ModelRegistry() :
	Singleton(this)
{
	LogStream::Initialize();

	_importer = new Assimp::Importer();
}

ModelRegistry::~ModelRegistry()
{
	delete _importer;
}

auto stripFirstDir(const std::filesystem::path path) -> std::filesystem::path
{
	const auto relPath = path.relative_path();
	if (relPath.empty()) return {};
	return relPath.lexically_relative(*relPath.begin());
}


auto ModelRegistry::Import(const std::filesystem::path& path) -> std::shared_ptr<Model>
{
	auto fullpath = BasePath;
	fullpath += path;

	auto pathStr = fullpath.string();

	const auto result = _meshes.find(path.string());
	if (result != _meshes.end())
	{
		const auto existingMesh = std::dynamic_pointer_cast<Model>(result->second);
		//const auto vb = existingMesh->_vertexBuffer;
		//const auto ib = existingMesh->_indexBuffer;

		auto newMesh = std::shared_ptr<Model>(new Model());
		return newMesh;
	}


	// Create a new mesh from file

	auto* scene = _importer->ReadFile(fullpath.string(), DefaultImportFlags);
	if (scene == nullptr || !scene->HasMeshes())
	{
		throw SaffronException(std::format("Failed to load mesh {}", path));
	}

	const auto meshCount = scene->mNumMeshes;

	std::vector<MeshVertex> meshVertices;
	std::vector<MeshFace> meshFaces;
	std::vector<SubMesh> subMeshes;
	std::vector<std::map<ModelTextureMapType, std::shared_ptr<Texture>>> textures;

	subMeshes.reserve(meshCount);

	uint vertexCount = 0;
	uint indexCount = 0;

	for (int meshIndex = 0; meshIndex < meshCount; meshIndex++)
	{
		auto* aiSubmesh = scene->mMeshes[meshIndex];

		if (!aiSubmesh->HasPositions())
		{
			throw SaffronException("Meshes must have positions");
		}

		SubMesh subMesh;
		subMesh.BaseVertex = vertexCount;
		subMesh.BaseIndex = indexCount;
		subMesh.VertexCount = aiSubmesh->mNumVertices;
		subMesh.IndexCount = aiSubmesh->mNumFaces * 3;
		subMesh.MeshName = aiSubmesh->mName.C_Str();
		subMesh.MaterialIndex = aiSubmesh->mMaterialIndex;
		subMeshes.emplace_back(subMesh);

		vertexCount += subMesh.VertexCount;
		indexCount += subMesh.IndexCount;

		meshVertices.reserve(vertexCount);
		meshFaces.reserve(aiSubmesh->mNumFaces);

		// Vertices
		for (int vertexIndex = 0; vertexIndex < subMesh.VertexCount; vertexIndex++)
		{
			const auto& v = aiSubmesh->mVertices[vertexIndex];
			meshVertices.emplace_back(Vector3{v.x, v.y, v.z});
		}

		// Faces
		for (int faceIndex = 0; faceIndex < aiSubmesh->mNumFaces; faceIndex++)
		{
			const auto& face = aiSubmesh->mFaces[faceIndex];
			if (face.mNumIndices != 3)
			{
				throw SaffronException("Faces must have 3 indices");
			}

			meshFaces.emplace_back(MeshFace{face.mIndices[0], face.mIndices[1], face.mIndices[2]});
		}

		// Normals
		if (aiSubmesh->HasNormals())
		{
			for (int normalIndex = 0; normalIndex < subMesh.VertexCount; normalIndex++)
			{
				const auto& v = aiSubmesh->mNormals[normalIndex];

				// We know that vertices has been added in previous loops
				meshVertices[subMesh.BaseVertex + normalIndex].Normal = Vector3{v.x, v.y, v.z};
			}
		}

		// TexCoords
		if (aiSubmesh->HasTextureCoords(0))
		{
			for (int texCoordIndex = 0; texCoordIndex < subMesh.VertexCount; texCoordIndex++)
			{
				const auto& v = aiSubmesh->mTextureCoords[0][texCoordIndex];

				// We know that vertices has been added in previous loops
				meshVertices[subMesh.BaseVertex + texCoordIndex].TexCoord = Vector2{v.x, 1.0f - v.y};
			}
		}

		// Tangent and bitangents
		if (aiSubmesh->HasTangentsAndBitangents())
		{
			for (int tangentIndex = 0; tangentIndex < subMesh.VertexCount; tangentIndex++)
			{
				const auto& v = aiSubmesh->mTangents[tangentIndex];

				// We know that vertices has been added in previous loops
				meshVertices[subMesh.BaseVertex + tangentIndex].Tangent = Vector3{v.x, v.y, v.z};
			}
		}
	}


	std::vector<std::shared_ptr<Material>> materials;

	if (scene->HasMaterials())
	{
		auto [importedMaterials, importedMaterialTextures] = ImportMaterials(*scene, fullpath);
		materials = std::move(importedMaterials);
		textures = std::move(importedMaterialTextures);
	}

	const auto layout = MeshVertex::VertexLayout();

	VertexStorage storage(layout);

	storage.Add(meshVertices);

	//auto vs = Shader::Create("MeshShader");
	//auto vb = VertexBuffer::Create(storage);
	//auto ib = IndexBuffer::Create(reinterpret_cast<const uint*>(meshFaces.data()), meshFaces.size() * 3);
	//auto il = InputLayout::Create(layout, vs);
	//auto sa = Sampler::Create();
	auto newMesh = std::shared_ptr<Model>(new Model());

	//newMesh->_shader = std::move(vs);
	//newMesh->_vertexBuffer = std::move(vb);
	//newMesh->_indexBuffer = std::move(ib);
	//newMesh->_inputLayout = std::move(il);
	//newMesh->_sampler = std::move(sa);
	//newMesh->_materials = std::move(materials);

	//newMesh->_subMeshes = std::move(subMeshes);
	//newMesh->_textures = std::move(textures);

	//newMesh->_name = path.stem().string();
	//newMesh->_path = path;

	//for (const auto& texCont : newMesh->_textures)
	//{
	//	auto& nativeSrvCont = newMesh->_nativeTextureSrvs.emplace_back();
	//	for (const auto& [type, tex] : texCont)
	//	{
	//		nativeSrvCont[static_cast<int>(type)] = &tex->ShaderView();
	//	}
	//}

	return newMesh;
}

auto ModelRegistry::ImportMaterials(const aiScene& scene,
                                    const std::filesystem::path& fullpath) const -> std::tuple<
	std::vector<std::shared_ptr<Material>>, std::vector<MatTexContainer>>
{
	const auto materialCount = scene.mNumMaterials;

	Debug::Assert(materialCount > 0);

	std::vector<std::shared_ptr<Material>> materials;
	materials.reserve(materialCount);

	std::vector<MatTexContainer> matTexContainer;

	for (int materialIndex = 0; materialIndex < materialCount; materialIndex++)
	{
		auto* const aiMaterial = scene.mMaterials[materialIndex];
		auto [material, materialTextures] = ImportMaterial(*aiMaterial, fullpath);

		materials.emplace_back(material);
		matTexContainer.emplace_back(std::move(materialTextures));
	}

	return std::make_tuple(materials, matTexContainer);
}

auto ModelRegistry::ImportMaterial(aiMaterial& aiMaterial,
                                   const std::filesystem::path& fullpath) const -> std::tuple<
	std::shared_ptr<Material>, MatTexContainer>
{
	MatTexContainer matTexContainer;
	const auto materialName = std::string(aiMaterial.GetName().C_Str());

	//auto material = Material::Create(_modelShader, materialName);
	//material->CBuffer().SetBindFlags(BindFlag_PS);

	//const auto parPath = fullpath.parent_path();

	//MaterialDataCBuf materialData;

	//// Albedo color
	//aiColor3D aiAlbedoColor;
	//if (aiMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, aiAlbedoColor) == AI_SUCCESS)
	//{
	//	materialData.AlbedoColor = Color{aiAlbedoColor.r, aiAlbedoColor.g, aiAlbedoColor.b, 1.0f};
	//}

	//// Emission
	//aiColor3D aiEmission;
	//if (aiMaterial.Get(AI_MATKEY_COLOR_EMISSIVE, aiEmission) == AI_SUCCESS)
	//{
	//	materialData.Emission = aiEmission.r;
	//}

	//// Shininess (temporary for later calculation)
	//float shininess;
	//if (aiMaterial.Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS)
	//{
	//	shininess = 80.0f;
	//}


	//// Reflectivity (temporary for later calculation)
	//float metalness;
	//if (aiMaterial.Get(AI_MATKEY_REFLECTIVITY, metalness) != AI_SUCCESS)
	//{
	//	metalness = 0.0f;
	//}


	////// Probe texture maps

	//aiString aiTexPath;

	//// Albedo map
	//const auto hasAlbedoMap = aiMaterial.GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
	//bool albedoMapFallback = !hasAlbedoMap;

	//if (hasAlbedoMap)
	//{
	//	std::filesystem::path texPath = parPath;
	//	texPath /= aiTexPath.C_Str();

	//	TextureSpec spec;
	//	spec.SRGB = true;
	//	spec.CreateSampler = false;

	//	// Force create texture since we need to know if fails or not
	//	Renderer::BeginStrategy(RenderStrategy::Immediate);
	//	auto texture = Texture::Create(texPath, spec);
	//	Renderer::EndStrategy();

	//	if (texture->Loaded())
	//	{
	//		matTexContainer.emplace(ModelTextureMapType::Albedo, texture);
	//		materialData.AlbedoColor = Colors::White;
	//	}
	//	else
	//	{
	//		albedoMapFallback = true;
	//	}
	//}

	//if (albedoMapFallback)
	//{
	//	matTexContainer.emplace(ModelTextureMapType::Albedo, Renderer::WhiteTexture());
	//}


	//// Normal map
	//const auto hasNormalMap = aiMaterial.GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS;
	//bool normalMapFallback = !hasNormalMap;

	//if (hasNormalMap)
	//{
	//	std::filesystem::path texPath = parPath;
	//	texPath /= aiTexPath.C_Str();

	//	// Force create texture since we need to know if fails or not
	//	Renderer::BeginStrategy(RenderStrategy::Immediate);
	//	auto texture = Texture::Create(texPath, {.CreateSampler = false});
	//	Renderer::EndStrategy();

	//	if (texture->Loaded())
	//	{
	//		matTexContainer.emplace(ModelTextureMapType::Normal, texture);
	//		materialData.UseNormalMap = true;
	//	}
	//	else
	//	{
	//		normalMapFallback = true;
	//	}
	//}

	//if (normalMapFallback)
	//{
	//	matTexContainer.emplace(ModelTextureMapType::Normal, Renderer::WhiteTexture());
	//	materialData.UseNormalMap = false;
	//}

	//const auto hasRoughtnessMap = aiMaterial.GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS ||
	//	aiMaterial.GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &aiTexPath) == AI_SUCCESS;
	//bool roughnessMapFallback = !hasRoughtnessMap;

	//if (hasRoughtnessMap)
	//{
	//	std::filesystem::path texPath = parPath;
	//	texPath /= aiTexPath.C_Str();

	//	// Force create texture since we need to know if fails or not
	//	Renderer::BeginStrategy(RenderStrategy::Immediate);
	//	auto texture = Texture::Create(texPath, {.CreateSampler = false});
	//	Renderer::EndStrategy();

	//	if (texture->Loaded())
	//	{
	//		matTexContainer.emplace(ModelTextureMapType::Roughness, texture);
	//		materialData.Roughness = 1.0f;
	//	}
	//	else
	//	{
	//		roughnessMapFallback = true;
	//	}
	//}

	//if (roughnessMapFallback)
	//{
	//	const float roughness = 1.0f - std::sqrt(shininess / 100.0f);

	//	matTexContainer.emplace(ModelTextureMapType::Roughness, Renderer::WhiteTexture());
	//	materialData.Roughness = roughness;
	//}


	//// Metalness texture is handled differently


	//materialData.Metalness = metalness;

	//bool hasMetalnessMap = false;

	//for (int i = 0; i < aiMaterial.mNumProperties; i++)
	//{
	//	const auto& prop = *aiMaterial.mProperties[i];

	//	if (prop.mType == aiPTI_String)
	//	{
	//	}
	//}

	//bool metalnessFallback = !hasMetalnessMap;

	//if (metalnessFallback)
	//{
	//	matTexContainer.emplace(ModelTextureMapType::Metalness, Renderer::WhiteTexture());
	//}


	//material->SetMaterialData(materialData);

	return std::make_tuple(Material::Create("NOT IMPLEMENTED"), matTexContainer);
}

auto ModelRegistry::CreateDefaultMaterial() -> std::shared_ptr<Material>
{
	static constexpr const auto* MaterialName = "Saffron-Default";

	const auto mat = Material::Create(MaterialName);
	const MaterialDataCBuf matData{
		.AlbedoColor = Color{0.8f, 0.8f, 0.8f}, .Metalness = 0.0f, .Roughness = 0.8f, .Emission = 0.0f,
		.EnvMapRotation = 0.0f, .UseNormalMap = false
	};
	//mat->SetMaterialData(matData);

	return mat;
}
}
