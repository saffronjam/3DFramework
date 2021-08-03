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
	_vertexShader->Bind();
	_pixelShader->Bind();
}

auto Mesh::SubMeshes() const -> const std::vector<SubMesh>&
{
	return _subMeshes;
}

auto Mesh::Create(const std::filesystem::path& path) -> std::shared_ptr<Mesh>
{
	return MeshStore::Instance().Import(path);
}

Mesh::Mesh()
{
}
}
