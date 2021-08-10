#include "SaffronPCH.h"

#include "Saffron/Graphics/Scene.h"

#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Mesh.h"

namespace Se
{
Scene::Scene() :
	_sceneRenderer(*this),
	_pointLightCBuffer(PointLightCBuffer{{}, 0}, 1)
{
	_sampleMesh = Mesh::Create("Mario.fbx");
	_sampleMesh->Transform() *= Matrix::CreateScale(0.01f);

	Renderer().ViewportResized += [this](const SizeEvent& event)
	{
		ViewportResized.Invoke(event);
		return false;
	};
}

void Scene::OnUpdate()
{
}

void Scene::OnRender(const CameraData& cameraData)
{
	_sceneRenderer.Begin(cameraData);

	// Submit every mesh in the scene to respective channel
	_sceneRenderer.SubmitMesh(_sampleMesh, RenderChannel_Geometry);

	_sceneRenderer.End();
}

auto Scene::Renderer() const -> const SceneRenderer&
{
	return _sceneRenderer;
}

void Scene::SetViewportSize(uint width, uint height)
{
	_sceneRenderer.SetViewportSize(width, height);
}
}
