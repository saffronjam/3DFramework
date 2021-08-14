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
	_commonCBuffer(ConstantBuffer<CommonCBuffer>::Create({}, 1)),
	_pointLight({Matrix::Identity, Vector3{1.0f, 0.0f, 0.0f}})
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

	_cameraMesh = Mesh::Create("Sphere.fbx");

	_pointLight.Position = Vector3{0.0f, 5.0f, 0.0f};

	Renderer().ViewportResized += [this](const SizeEvent& event)
	{
		ViewportResized.Invoke(event);
		return false;
	};
}

void Scene::OnUpdate(TimeSpan ts)
{
	_activeCamera->OnUpdate(ts);
}

void Scene::OnRender()
{
	const auto& timer = App::Instance().Timer();
	//_pointLight.Position = Vector3{0.0f, std::sin(timer.SinceStart().Sec() * 8.0f) * 3.0f, 0.0f};

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


	_commonCBuffer->Update({_activeCamera->Data().Position});
	_commonCBuffer->Bind();


	_sceneRenderer.Begin(_activeCamera->Data());

	if (_drawCameraFrustums)
	{
		if (&_camera1 != _activeCamera)
		{
			_sceneRenderer.SubmitCameraFrustum(_camera1, _camera1.View());
		}
		if (&_camera2 != _activeCamera)
		{
			_sceneRenderer.SubmitCameraFrustum(_camera2, _camera2.View());
		}
	}

	_sceneRenderer.BeginSubmtions(RenderChannel_Geometry | RenderChannel_Shadow);
	for (auto& mesh : _sampleMeshes)
	{
		_sceneRenderer.SubmitMesh(mesh);
	}
	_sceneRenderer.SubmitMesh(_samplePlane);
	_sceneRenderer.EndSubmtions();

	_sceneRenderer.BeginSubmtions(RenderChannel_Geometry);

	_sceneRenderer.SubmitMesh(_sampleSphere);
	_sceneRenderer.SubmitMesh(
		_cameraMesh,
		Matrix::CreateScale(0.4) * Matrix::CreateTranslation(_camera1.Data().Position)
	);
	_sceneRenderer.SubmitMesh(
		_cameraMesh,
		Matrix::CreateScale(0.4) * Matrix::CreateTranslation(_camera2.Data().Position)
	);

	_sceneRenderer.EndSubmtions();

	_sceneRenderer.End();
}

void Scene::OnUi()
{
	ImGui::Begin("Scene");
	ImGui::SliderFloat3("", reinterpret_cast<float*>(&_pointLight.Position), -2.0f, 15.0f);
	if (ImGui::RadioButton("Camera1", &_activeRadioButton, 0))
	{
		_activeCamera = &_camera1;
	}
	if (ImGui::RadioButton("Camera2", &_activeRadioButton, 1))
	{
		_activeCamera = &_camera2;
	}
	ImGui::Checkbox("Frustums", &_drawCameraFrustums);

	ImGui::End();

	_activeCamera->OnUi();
	_sceneRenderer.OnUi();
}

auto Scene::Renderer() const -> const SceneRenderer&
{
	return _sceneRenderer;
}

auto Scene::ViewportWidth() const -> uint
{
	return _viewportWidth;
}

auto Scene::ViewportHeight() const -> uint
{
	return _viewportHeight;
}

void Scene::SetViewportSize(uint width, uint height)
{
	_sceneRenderer.SetViewportSize(width, height);

	_camera1.SetProjection({Math::Pi / 4.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f});
	_camera2.SetProjection({Math::Pi / 4.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f});

	_viewportWidth = width;
	_viewportHeight = height;
}
}
