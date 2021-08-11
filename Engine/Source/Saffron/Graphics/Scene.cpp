#include "SaffronPCH.h"

#include "Saffron/Graphics/Scene.h"

#include "Saffron/Common/App.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Math/Random.h"

namespace Se
{
Scene::Scene() :
	_sceneRenderer(*this),
	_pointLightCBuffer(ConstantBuffer<PointLightCBuffer>::Create({}, 2)),
	_commonCBuffer(ConstantBuffer<CommonCBuffer>::Create({}, 1)),
	_pointLight(Vector3{1.0f, 0.0f, 0.0f})
{
	for (int i = 0; i < 10; i++)
	{
		auto mesh = Mesh::Create("Torus.fbx");
		_sampleMeshes.emplace_back(mesh);
		_sampleMeshesPosition.emplace_back(Random::Vec3({-1.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f}) * 6.0f);
	}

	_sampleSphere = Mesh::Create("Sphere.fbx");
	_sampleSphere->SetShader(Shader::Create("Transform"));
	_samplePlane = Mesh::Create("Plane.fbx");
	_samplePlane->Transform() = Matrix::CreateScale(10.0f) * Matrix::CreateTranslation(0.0f, -3.0f, 0.0f);

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
	_pointLight.Position = Vector3{0.0f, std::sin(timer.SinceStart().Sec() * 16.0f) * 3.0f, 0.0f};

	float index = 0;
	for (const auto& mesh : _sampleMeshes)
	{
		mesh->Transform() = Matrix::CreateTranslation(_sampleMeshesPosition[static_cast<int>(index)]) *
			Matrix::CreateTranslation(
				{0.0f, std::sin(timer.SinceStart().Sec() * 8.0f * (1 + index / _sampleMeshes.size())) * 3.0f, 0.0f}
			);
		index++;
	}

	_sceneRenderer.SceneCommon().PointLight = _pointLight;

	_sampleSphere->Transform() = Matrix::CreateScale(0.02f) * Matrix::CreateTranslation(_pointLight.Position);

	_pointLightCBuffer->Data().PointLights[0] = _pointLight;
	_pointLightCBuffer->Data().nPointLights = 1;
	_pointLightCBuffer->UploadData();

	_commonCBuffer->Update({cameraData.Position});


	_pointLightCBuffer->Bind();
	_commonCBuffer->Bind();

	_sceneRenderer.Begin(cameraData);

	// Submit every mesh in the scene to respective channel
	for (auto& mesh : _sampleMeshes)
	{
		_sceneRenderer.SubmitMesh(mesh, RenderChannel_Geometry | RenderChannel_Shadow);
	}
	_sceneRenderer.SubmitMesh(_sampleSphere, RenderChannel_Geometry);
	_sceneRenderer.SubmitMesh(_samplePlane, RenderChannel_Geometry | RenderChannel_Shadow);

	_sceneRenderer.End();
}

void Scene::OnUi()
{
	_sceneRenderer.OnUi();
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
