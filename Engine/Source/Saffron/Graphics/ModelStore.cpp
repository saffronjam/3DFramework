#include "SaffronPCH.h"

#define NOMINMAX

#include "Saffron/Graphics/ModelStore.h"

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Saffron/Graphics/Model.h"
#include "Saffron/Graphics/Material.h"

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
const std::filesystem::path ModelStore::BasePath("Assets/Meshes/");

const uint ModelStore::DefaultImportFlags = aiProcess_CalcTangentSpace | // Create binormals/tangents just in case
	aiProcess_Triangulate | // Make sure we're triangles
	aiProcess_SortByPType | // Split meshes by primitive type
	aiProcess_GenNormals | // Make sure we have legit normals
	aiProcess_GenUVCoords | // Convert UVs if required 
	aiProcess_OptimizeMeshes | // Batch draws where possible
	aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure; // Validation

ModelStore::ModelStore() :
	Singleton(this),
	_meshShader(Shader::Create("MeshShader"))
{
	LogStream::Initialize();

	_importer = new Assimp::Importer();
}

ModelStore::~ModelStore()
{
	delete _importer;
}

auto stripFirstDir(const std::filesystem::path path) -> std::filesystem::path
{
	const auto relPath = path.relative_path();
	if (relPath.empty()) return {};
	return relPath.lexically_relative(*relPath.begin());
}


auto ModelStore::Import(const std::filesystem::path& path) -> std::shared_ptr<Model>
{
	auto fullpath = BasePath;
	fullpath += path;

	auto pathStr = fullpath.string();

	const auto result = _meshes.find(path.string());
	if (result != _meshes.end())
	{
		const auto existingMesh = std::dynamic_pointer_cast<Model>(result->second);
		const auto vb = existingMesh->_vertexBuffer;
		const auto ib = existingMesh->_indexBuffer;

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
	const auto materialCount = scene->mNumMaterials;

	std::vector<MeshVertex> meshVertices;
	std::vector<MeshFace> meshFaces;
	std::vector<SubMesh> subMeshes;
	std::vector<std::map<ModelTextureMapType, std::shared_ptr<Texture>>> textures;
	std::vector<std::shared_ptr<Material>> materials;

	subMeshes.reserve(meshCount);
	materials.reserve(materialCount);

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


	if (scene->HasMaterials())
	{
		auto texBasePath = fullpath;
		texBasePath.remove_filename();
		for (int materialIndex = 0; materialIndex < materialCount; materialIndex++)
		{
			auto& texCont = textures.emplace_back();

			auto* const aiMaterial = scene->mMaterials[materialIndex];
			const auto aiMaterialName = std::string(aiMaterial->GetName().C_Str());

			auto material = Material::Create(_meshShader, aiMaterialName);
			material->CBuffer().SetBindFlags(ConstantBufferBindFlags_PS);
			materials.emplace_back(material);


			MaterialDataCBuf materialData;


			const auto textureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);

			// Albedo color
			aiColor3D aiAlbedoColor;
			if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiAlbedoColor) == AI_SUCCESS)
			{
				materialData.AlbedoColor = Color{aiAlbedoColor.r, aiAlbedoColor.g, aiAlbedoColor.b, 1.0f};
			}

			// Emission
			aiColor3D aiEmission;
			if (aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmission) == AI_SUCCESS)
			{
				materialData.Emission = aiEmission.r;
			}

			// Shininess (temporary for later calculation)
			float shininess;
			if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS)
			{
				shininess = 80.0f;
			}

			// Reflectivity (temporary for later calculation)
			float reflectivity;
			if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity) != AI_SUCCESS)
			{
				reflectivity = 80.0f;
			}


			//// Probe texture maps

			aiString aiTexPath;

			// Albedo map
			const auto hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
			bool albedoMapFallback = !hasAlbedoMap;

			if (hasAlbedoMap)
			{
				std::filesystem::path texPath = fullpath.parent_path();
				texPath /= aiTexPath.C_Str();

				TextureSpec spec;
				spec.SRGB = true;
				spec.CreateSampler = false;

				// Force create texture since we need to know if fails or not
				Renderer::BeginStrategy(RenderStrategy::Immediate);
				auto texture = Texture::Create(texPath, spec);
				Renderer::EndStrategy();

				if (texture->Loaded())
				{
					texCont.emplace(ModelTextureMapType::Albedo, texture);
					materialData.AlbedoColor = Colors::White;
				}
				else
				{
					albedoMapFallback = true;
				}
			}

			if (albedoMapFallback)
			{
				texCont.emplace(ModelTextureMapType::Albedo, Renderer::WhiteTexture());
			}

			// Normal map
			const auto hasNormalMap = aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS;
			bool normalMapFallback = !hasNormalMap;

			if (hasNormalMap)
			{
				std::filesystem::path texPath = fullpath.parent_path();
				texPath /= aiTexPath.C_Str();

				// Force create texture since we need to know if fails or not
				Renderer::BeginStrategy(RenderStrategy::Immediate);
				auto texture = Texture::Create(texPath, {.CreateSampler = false});
				Renderer::EndStrategy();

				if (texture->Loaded())
				{
					texCont.emplace(ModelTextureMapType::Normal, texture);
					materialData.UseNormalMap = true;
				}
				else
				{
					normalMapFallback = true;
				}
			}

			if (normalMapFallback)
			{
				texCont.emplace(ModelTextureMapType::Normal, Renderer::WhiteTexture());
				materialData.UseNormalMap = false;
			}

			const auto hasRoughtnessMap = aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS;
			bool roughnessMapFallback = !hasRoughtnessMap;

			if (hasRoughtnessMap)
			{
				std::filesystem::path texPath = fullpath.parent_path();
				texPath /= aiTexPath.C_Str();

				// Force create texture since we need to know if fails or not
				Renderer::BeginStrategy(RenderStrategy::Immediate);
				auto texture = Texture::Create(texPath, {.CreateSampler = false});
				Renderer::EndStrategy();

				if (texture->Loaded())
				{
					texCont.emplace(ModelTextureMapType::Roughness, texture);
					materialData.Roughness = 1.0f;
				}
				else
				{
					roughnessMapFallback = true;
				}
			}

			if (roughnessMapFallback)
			{
				const float roughness = 1.0f - std::sqrt(shininess / 100.0f);

				texCont.emplace(ModelTextureMapType::Roughness, Renderer::WhiteTexture());
				materialData.Roughness = roughness;
			}

			// TODO: Metalness
			materialData.Metalness = 1.0f;

			material->SetMaterialData(materialData);
		}
	}


	const auto layout = MeshVertex::VertexLayout();

	VertexStorage storage(layout);

	storage.Add(meshVertices);

	auto vs = Shader::Create("MeshShader");
	auto vb = VertexBuffer::Create(storage);
	auto ib = IndexBuffer::Create(reinterpret_cast<const uint*>(meshFaces.data()), meshFaces.size() * 3);
	auto il = InputLayout::Create(layout, vs);
	auto sa = Sampler::Create();
	auto newMesh = std::shared_ptr<Model>(new Model());

	newMesh->_shader = std::move(vs);
	newMesh->_vertexBuffer = std::move(vb);
	newMesh->_indexBuffer = std::move(ib);
	newMesh->_inputLayout = std::move(il);
	newMesh->_sampler = std::move(sa);
	newMesh->_materials = std::move(materials);

	newMesh->_subMeshes = std::move(subMeshes);
	newMesh->_textures = std::move(textures);

	for (const auto& texCont : newMesh->_textures)
	{
		auto& nativeSrvCont = newMesh->_nativeTextureSrvs.emplace_back();
		for (const auto& [type, tex] : texCont)
		{
			nativeSrvCont[static_cast<int>(type)] = &tex->ShaderView();
		}
	}

	return newMesh;
}
}
