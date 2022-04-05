#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneEnvironment.h"

#include "Saffron/Rendering/Renderer.h"
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
	Renderer::Submit(
		[](const RendererPackage& package)
		{
			const auto envMapSize = Renderer::Config().EnvironmentMapResolution;
		}
	);
}

std::shared_ptr<SceneEnvironment> SceneEnvironment::Create(const std::filesystem::path& path)
{
	const auto envMapSize = Renderer::Config().EnvironmentMapResolution;
	const auto inst = std::make_shared<SceneEnvironment>();


	return inst;
}
}
