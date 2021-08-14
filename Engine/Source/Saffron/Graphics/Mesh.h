#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindables.h"

namespace Se
{
struct MeshVertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 Binormal;
	Vector2 TexCoord;

	static auto VertexLayout() -> class VertexLayout
	{
		return {
			{"Position", ElementType::Float3}, {"Normal", ElementType::Float3}, {"Tangent", ElementType::Float3},
			{"Binormal", ElementType::Float3}, {"TexCoord", ElementType::Float2}
		};
	}
};

struct MeshFace
{
	uint x, y, z;
};

struct SubMesh
{
	uint BaseVertex;
	uint BaseIndex;
	uint MaterialIndex;
	uint IndexCount;
	uint VertexCount;

	Matrix Transform;

	std::string NodeName, MeshName;
};

class Mesh
{
	friend class MeshStore;
public:
	void Bind();

	auto SubMeshes() const -> const std::vector<SubMesh>&;
	auto Textures() const -> const std::vector<std::shared_ptr<Texture>>&;
	auto Transform() -> Matrix&;
	auto Transform() const -> const Matrix&;

	// Until materials are implemented
	void SetShader(const std::shared_ptr<Shader>& shader);
	
	static auto Create(const std::filesystem::path& path) -> std::shared_ptr<Mesh>;

private:
	Mesh();

private:
	std::shared_ptr<VertexBuffer> _vertexBuffer;
	std::shared_ptr<IndexBuffer> _indexBuffer;
	std::shared_ptr<InputLayout> _inputLayout;
	std::shared_ptr<Shader> _shader;
	std::shared_ptr<Sampler> _sampler;

	std::vector<SubMesh> _subMeshes;
	std::vector<std::shared_ptr<Texture>> _textures;

	Matrix _transform;
};
}
