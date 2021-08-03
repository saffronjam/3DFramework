#include "SaffronPCH.h"

#define NOMINMAX

#include "Saffron/Graphics/MeshStore.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Saffron/Graphics/Mesh.h"

namespace Se
{
const std::filesystem::path MeshStore::BasePath("Assets/Meshes/");

const uint MeshStore::DefaultImportFlags = aiProcess_CalcTangentSpace | // Create binormals/tangents just in case
	aiProcess_Triangulate | // Make sure we're triangles
	aiProcess_SortByPType | // Split meshes by primitive type
	aiProcess_GenNormals | // Make sure we have legit normals
	aiProcess_GenUVCoords | // Convert UVs if required 
	aiProcess_OptimizeMeshes | // Batch draws where possible
	aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure; // Validation

MeshStore::MeshStore() :
	SingleTon(this)
{
	_importer = new Assimp::Importer();
}

MeshStore::~MeshStore()
{
	delete _importer;
}

auto MeshStore::Import(const std::filesystem::path& path) -> std::shared_ptr<Mesh>
{
	auto fullpath = BasePath;
	fullpath += path;

	auto pathStr = fullpath.string();

	const auto result = _meshes.find(path.string());
	if (result != _meshes.end())
	{
		const auto existingMesh = std::dynamic_pointer_cast<Mesh>(result->second);
		const auto vb = existingMesh->_vertexBuffer;
		const auto ib = existingMesh->_indexBuffer;

		auto newMesh = std::shared_ptr<Mesh>(new Mesh());
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
	std::vector<SubMesh> _subMeshes;

	_subMeshes.reserve(meshCount);

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
		_subMeshes.emplace_back(subMesh);

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
	}

	const auto layout = MeshVertex::VertexLayout();

	VertexStorage storage(layout);

	storage.Add(meshVertices);

	auto vs = VertexShader::Create("MeshShader_v");
	auto ps = PixelShader::Create("MeshShader_p");
	auto vb = VertexBuffer::Create(storage);
	auto ib = IndexBuffer::Create(reinterpret_cast<const uint*>(meshFaces.data()), meshFaces.size() * 3);
	auto il = InputLayout::Create(layout, vs);

	auto newMesh = std::shared_ptr<Mesh>(new Mesh());

	newMesh->_vertexShader = std::move(vs);
	newMesh->_pixelShader = std::move(ps);
	newMesh->_vertexBuffer = std::move(vb);
	newMesh->_indexBuffer = std::move(ib);
	newMesh->_inputLayout = std::move(il);
	newMesh->_subMeshes = std::move(_subMeshes);

	return newMesh;
}
}
