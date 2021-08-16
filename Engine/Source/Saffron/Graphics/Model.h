#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/Material.h"
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

enum class ModelTextureMapType
{
	Albedo = 0,
	Normal = 1,
	Roughness = 2,
	Metalness = 3,
	Count
};

class Model : public Managed<Model>
{
	friend class ModelStore;
public:
	void Bind();
	void OnDebugUi();

	auto SubMeshes() const -> const std::vector<SubMesh>&;
	auto Materials() const -> const std::vector<std::shared_ptr<Material>>&;
	auto MaterialTextures() const -> const std::vector<std::map<ModelTextureMapType, std::shared_ptr<Texture>>>&;
	auto MaterialTextureShaderViews(int materialIndex) const -> const std::array<ID3D11ShaderResourceView*, static_cast<int>(ModelTextureMapType::Count)>&;
	auto Transform() -> Matrix&;
	auto Transform() const -> const Matrix&;

	// Until materials are implemented
	void SetShader(const std::shared_ptr<Shader>& shader);

	static auto Create(const std::filesystem::path& path) -> std::shared_ptr<Model>;

private:
	Model();

private:
	std::shared_ptr<VertexBuffer> _vertexBuffer;
	std::shared_ptr<IndexBuffer> _indexBuffer;
	std::shared_ptr<InputLayout> _inputLayout;
	std::shared_ptr<Shader> _shader;
	std::shared_ptr<Sampler> _sampler;
	std::vector<std::shared_ptr<Material>> _materials;

	std::vector<SubMesh> _subMeshes;

	// Materials
	std::vector<std::map<ModelTextureMapType, std::shared_ptr<Texture>>> _textures;
	std::vector<std::array<ID3D11ShaderResourceView*, static_cast<int>(ModelTextureMapType::Count)>> _nativeTextureSrvs = {};

	Matrix _transform;
};
}
