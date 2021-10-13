#include "SaffronPCH.h"

#include "Saffron/Graphics/Model.h"

#include "Saffron/Graphics/ModelStore.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
void Model::Bind()
{
	_vertexBuffer->Bind();
	_indexBuffer->Bind();
	_inputLayout->Bind();

	const auto inst = ShareThisAs<Model>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			package.Context.PSSetShaderResources(
				0,
				inst->_nativeTextureSrvs.size(),
				inst->_nativeTextureSrvs.data()->data()
			);
		}
	);
	_sampler->Bind();
}

void Model::OnDebugUi()
{
	ImGui::Begin("Model");
	for (auto& cont : _textures)
	{
		auto type = ModelTextureMapType::Roughness;
		if (cont[type] != nullptr)
		{
			Ui::Image(*cont[type]);
		}
	}
	ImGui::End();
}

auto Model::SubMeshes() const -> const std::vector<SubMesh>&
{
	return _subMeshes;
}

auto Model::Materials() const -> const std::vector<std::shared_ptr<Material>>&
{
	return _materials;
}

auto Model::MaterialTextures() const -> const std::vector<std::map<ModelTextureMapType, std::shared_ptr<Texture>>>&
{
	return _textures;
}

auto Model::MaterialTextureShaderViews(
	int materialIndex
) const -> const std::array<ID3D11ShaderResourceView*, static_cast<int>(ModelTextureMapType::Count)>&
{
	return _nativeTextureSrvs[materialIndex];
}

auto Model::Transform() -> Matrix&
{
	return _transform;
}

auto Model::Transform() const -> const Matrix&
{
	return _transform;
}

void Model::SetShader(const std::shared_ptr<Shader>& shader)
{
	_shader = shader;
}

auto Model::Create(const std::filesystem::path& path) -> std::shared_ptr<Model>
{
	return ModelStore::Instance().Import(path);
}

Model::Model()
{
}
}
