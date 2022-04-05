#include "SaffronPCH.h"

#include "Saffron/Graphics/Model.h"

#include "Saffron/Graphics/ModelRegistry.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
void Model::Bind()
{
}

void Model::Unbind()
{
}

void Model::OnDebugUi()
{
}

auto Model::SubMeshes() const -> const std::vector<SubMesh>&
{
	return _subMeshes;
}

auto Model::Materials() const -> const std::vector<std::shared_ptr<Material>>&
{
	return _materials;
}

auto Model::Transform() -> Matrix&
{
	return _transform;
}

auto Model::Transform() const -> const Matrix&
{
	return _transform;
}

auto Model::Name() const -> const std::string&
{
	return _name;
}

auto Model::Path() const -> const std::filesystem::path&
{
	return _path;
}

auto Model::Create(const std::filesystem::path& path) -> std::shared_ptr<Model>
{
	return ModelRegistry::Instance().Import(path);
}

Model::Model()
{
}
}
