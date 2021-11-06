#pragma once

#include "Renderer.h"
#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindables/Texture.h"
#include "Saffron/Ui/Ui.h"

namespace Se
{
class SceneEnvironment : public Managed<SceneEnvironment>
{
public:
	static std::shared_ptr<SceneEnvironment> Create(const std::filesystem::path& path);

	void Bind() const
	{
		_cubeMap->Bind();
	}

	void OnUi()
	{
		ImGui::Begin("Test");
		if(ImGui::Button("Reload equi"))
		{
			Draw();
		}
		ImGui::End();
	}

	void Draw() const
	{
		_cubeMap->SetUsage(TextureUsage_UnorderedAccess);
		_cubeMap->Bind();
		_equiTex->Bind();
		_generationShader->Bind();

		Renderer::Submit(
			[](const RendererPackage& package)
			{
				const auto envMapSize = Renderer::Config().EnvironmentMapResolution;
				package.Context.Dispatch(envMapSize / 32, envMapSize / 32, 6);
			}
		);
		_cubeMap->Unbind();
		_cubeMap->SetUsage(TextureUsage_ShaderResource);
	}

private:
	std::shared_ptr<TextureCube> _cubeMap;
	std::shared_ptr<Texture> _equiTex;
	std::shared_ptr<Shader> _generationShader;
};
}
