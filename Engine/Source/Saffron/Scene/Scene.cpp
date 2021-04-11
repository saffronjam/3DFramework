#include "SaffronPCH.h"

#include "Saffron/Math/Random.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/Scene.h"

#include "Saffron/Common/Timer.h"

namespace Se
{
Scene::Scene() :
	_cube(CreateShared<Mesh>())
{
}

void Scene::OnUpdate()
{
	//static float timerFloat = 0.0f;
	//timerFloat += Global::Timer::GetStep().sec() * 0.1f;
	//auto current = _pointLight->GetPosition();
	//current.y = std::sin(timerFloat) * 10.0f;
	//_pointLight->SetPosition(current);
}

void Scene::OnRender()
{
	SceneRenderer::Instance().Begin(GetShared());

	SceneRenderer::Instance().Submit(_cube, nullptr, RenderChannel::Main);

	SceneRenderer::Instance().End();
}

void Scene::OnGuiRender()
{
}

void Scene::SetViewMatrix(const Matrix& view)
{
	_cameraTransforms.MainView = view;
}

void Scene::SetProjectionMatrix(const Matrix& projection)
{
	_cameraTransforms.MainProjection = projection;
}

const Scene::CameraTransforms& Scene::GetCameraTransforms() const
{
	return _cameraTransforms;
}
}
