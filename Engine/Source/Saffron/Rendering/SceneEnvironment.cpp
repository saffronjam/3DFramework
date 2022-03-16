#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneEnvironment.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/Bindables/Texture.h"
#include "Saffron/Rendering/Binders/Binder.h"
#include "Saffron/Rendering/Binders/ShaderBindFlagsBinder.h"
#include "Saffron/Rendering/Binders/TextureUsageBinder.h"
#include "Saffron/Ui/Ui.h"

namespace Se
{
void SceneEnvironment::OnUi()
{
	ImGui::Begin("Test");
	if (ImGui::Button("Reload equi"))
	{
		Reload();
	}
	ImGui::End();
}

void SceneEnvironment::Reload() const
{
	ScopedTexUsageBinder(_cubeMap, TextureUsage_UnorderedAccess);
	ScopedBindFlagsBinder(_cubeMap, BindFlag_CS);
	ScopedBinder(_cubeMap);
	ScopedBinder(_equiTex);
	ScopedBinder(_generationShader);

	Renderer::Submit(
		[](const RendererPackage& package)
		{
			const auto envMapSize = Renderer::Config().EnvironmentMapResolution;
			package.Context.Dispatch(envMapSize / 32, envMapSize / 32, 6);
		}
	);
}

auto SceneEnvironment::Texture() const -> const std::shared_ptr<TextureCube>&
{
	return _cubeMap;
}

std::shared_ptr<SceneEnvironment> SceneEnvironment::Create(const std::filesystem::path& path)
{
	const auto envMapSize = Renderer::Config().EnvironmentMapResolution;
	const auto inst = std::make_shared<SceneEnvironment>();

	inst->_equiTex = Texture::Create(path);
	inst->_equiTex->SetBindFlags(BindFlag_CS);
	inst->_cubeMap = TextureCube::Create(
		envMapSize,
		envMapSize,
		ImageFormat::RGBA32f,
		{.Usage = TextureUsage_UnorderedAccess | TextureUsage_ShaderResource}
	);
	inst->_generationShader = Shader::Create("EquirectangularToCubeMap", true);


	inst->Reload();

	return inst;
}
}
