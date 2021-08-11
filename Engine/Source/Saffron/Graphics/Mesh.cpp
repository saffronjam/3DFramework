#include "SaffronPCH.h"

#include "Saffron/Graphics/Mesh.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Graphics/MeshStore.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
void Mesh::Bind()
{
	_vertexBuffer->Bind();
	_indexBuffer->Bind();
	_inputLayout->Bind();
	_shader->Bind();
	for (auto& texture : _textures)
	{
		texture->Bind();
	}
}

auto Mesh::SubMeshes() const -> const std::vector<SubMesh>&
{
	return _subMeshes;
}

auto Mesh::Textures() const -> const std::vector<std::shared_ptr<Texture>>&
{
	return _textures;
}

auto Mesh::Transform() -> Matrix&
{
	return _transform;
}

auto Mesh::Transform() const -> const Matrix&
{
	return _transform;
}

void Mesh::SetShader(const std::shared_ptr<Shader>& shader)
{
	_shader = shader;
}

auto Mesh::Create(const std::filesystem::path& path) -> std::shared_ptr<Mesh>
{
	return MeshStore::Instance().Import(path);
}

Mesh::Mesh()
{
}
}
