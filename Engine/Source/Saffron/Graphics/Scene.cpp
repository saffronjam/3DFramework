#include "SaffronPCH.h"

#include "Saffron/Graphics/Scene.h"

#include "Saffron/Common/App.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Mesh.h"

namespace Se
{
Scene::Scene() :
	_sceneRenderer(*this),
	_pointLightCBuffer(ConstantBuffer<PointLightCBuffer>::Create({}, 2)),
	_commonCBuffer(ConstantBuffer<CommonCBuffer>::Create({}, 1)),
	_pointLight(Vector3{1.0f, 0.0f, 0.0f})
{
	_sampleMesh = Mesh::Create("Torus.fbx");
	_sampleSphere = Mesh::Create("Sphere.fbx");
	_sampleSphere->SetShader(Shader::Create("Transform"));

	Renderer().ViewportResized += [this](const SizeEvent& event)
	{
		ViewportResized.Invoke(event);
		return false;
	};
}

void Scene::OnUpdate(TimeSpan ts)
{
}

void Scene::OnRender(const CameraData& cameraData)
{
	const auto& timer = App::Instance().Timer();
	_pointLight.Position =  Vector3{ 0.0f, std::sin(timer.SinceStart().Sec() * 10.0f), 0.0f };

	_sampleSphere->Transform() = Matrix::CreateScale(0.02f) * Matrix::CreateTranslation(_pointLight.Position);

	_pointLightCBuffer->Data().PointLights[0] = _pointLight;
	_pointLightCBuffer->Data().nPointLights = 1;
	_pointLightCBuffer->UploadData();

	_commonCBuffer->Update({cameraData.Position});


	_pointLightCBuffer->Bind();
	_commonCBuffer->Bind();

	_sceneRenderer.Begin(cameraData);

	// Submit every mesh in the scene to respective channel
	_sceneRenderer.SubmitMesh(_sampleMesh, RenderChannel_Geometry);
	_sceneRenderer.SubmitMesh(_sampleSphere, RenderChannel_Geometry);

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
