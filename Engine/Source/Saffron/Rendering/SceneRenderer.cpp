#include "SaffronPCH.h"

#include <bgfx/bgfx.h>

#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Graphs/DefaultRenderGraph.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
SceneRenderer* SceneRenderer::_instance = nullptr;

SceneRenderer::SceneRenderer() :
	_renderGraph(CreateUnique<DefaultRenderGraph>())
{
	SE_CORE_ASSERT(_instance == nullptr && "Scene renderer was already instansiated");
	_instance = this;

	_jobs.insert({RenderChannel::Main, {}});
	_jobs.insert({RenderChannel::Shadow, {}});
}

void SceneRenderer::Begin(const Shared<Scene>& scene)
{
	_activeScene = scene;
}

void SceneRenderer::End()
{
	_renderGraph->Execute(_jobs);
	_activeScene.reset();
}

void SceneRenderer::Submit(const Shared<Mesh>& mesh, const Shared<Material>& material, RenderChannel renderChannel)
{
	auto emplaceIfOnChannel = [&](const Shared<Mesh> mesh, RenderChannel check, RenderChannel match)
	{
		if (check & match != 0)
		{
			_jobs.at(check).emplace_back(mesh, material, Matrix::Identity);
		}
	};

	emplaceIfOnChannel(mesh, renderChannel, RenderChannel::Main);
	emplaceIfOnChannel(mesh, renderChannel, RenderChannel::Shadow);
}

Scene& SceneRenderer::GetActiveScene()
{
	return *_activeScene;
}

const Scene& SceneRenderer::GetActiveScene() const
{
	return *_activeScene;
}
}
