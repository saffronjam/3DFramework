#pragma once

#include "Saffron/Base.h"
#include "Saffron/Common/Enums.h"
#include "Saffron/Graphics/Material.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Rendering/VertexLayout.h"

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


class Model : public Managed<Model>
{
	friend class ModelRegistry;
public:
	void Bind();
	void Unbind();

	void OnDebugUi();

	auto SubMeshes() const -> const std::vector<SubMesh>&;
	auto Materials() const -> const std::vector<std::shared_ptr<Material>>&;
	auto Transform() -> Matrix&;
	auto Transform() const -> const Matrix&;

	auto Name() const -> const std::string&;
	auto Path() const -> const std::filesystem::path&;

	static auto Create(const std::filesystem::path& path) -> std::shared_ptr<Model>;

private:
	Model();

private:
	std::vector<std::shared_ptr<Material>> _materials;

	std::string _name;
	std::filesystem::path _path;

	std::vector<SubMesh> _subMeshes;
	
	Matrix _transform;
};
}
