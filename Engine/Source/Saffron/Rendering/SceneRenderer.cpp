#include "SaffronPCH.h"

#include <bgfx/bgfx.h>

#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
SceneRenderer* SceneRenderer::_instance = nullptr;

SceneRenderer::SceneRenderer() :
	_commonProgram(CreateShared<Program>("cubes"))
{
	SE_CORE_ASSERT(_instance == nullptr && "Scene renderer was already instansiated");
	_instance = this;
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::Begin(const Shared<Scene>& scene)
{
	_activeScene = scene;
}

void SceneRenderer::End()
{
	for (const auto& [channel, mesh] : _submitions)
	{
		mesh->_vertexBuffer->Bind();
		mesh->_indexBuffer->Bind();

		auto viewProj = _activeScene->GetCameraTransforms().MainView * _activeScene->GetCameraTransforms().
			MainProjection;
		bgfx::setTransform(viewProj.m);

		bgfx::submit(0, _commonProgram->GetNativeHandle());
	}

	_activeScene.reset();
}

void SceneRenderer::Submit(const Shared<Mesh>& mesh, RenderChannel renderChannel)
{
	_submitions.emplace(renderChannel, mesh);
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
