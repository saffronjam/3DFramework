#include "SaffronPCH.h"

#include "Saffron/Graphics/Scene.h"

#include "Saffron/Common/App.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Math/Random.h"

namespace Se
{
Scene::Scene() :
	_sceneRenderer(*this),
	_pointLight({Matrix::Identity, Vector3{1.0f, 0.0f, 0.0f}, 5.0f})
{
	_sampleModel = Model::Create("AntCamera/scene.gltf");
	_sponzaScene = Model::Create("Sponza/Sponza.gltf");
	_sampleSphere = Model::Create("Sphere.fbx");
	_sponzaScene->Transform() *= Matrix::CreateScale(0.01f);
	_sampleSphere->SetShader(Shader::Create("Transform"));

	_cameraMesh = Model::Create("Sphere.fbx");

	_pointLight.Position = Vector3{ 0.0f, 5.0f, 0.0f };
	_pointLight.Radius = 4.0f;
	_pointLight.Color = Colors::White;

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

	//_pointLight.Position = _activeCamera->Data().Position;

	_sceneRenderer.SceneCommon().PointLight = _pointLight;
	_sampleSphere->Transform() = Matrix::CreateScale(0.02f) * Matrix::CreateTranslation(_pointLight.Position);
	_sampleModel->Transform() = Matrix::CreateFromYawPitchRoll(0.0f, Math::Pi / 3.0f, Math::Pi / 5.0f) *
		Matrix::CreateTranslation(0.0f, 1.5f, 0.0f);



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
	_sceneRenderer.SubmitModel(_sponzaScene);
	_sceneRenderer.SubmitModel(_sampleModel);
	_sceneRenderer.EndSubmtions();

	_sceneRenderer.BeginSubmtions(RenderChannel_Geometry);

	_sceneRenderer.SubmitModel(_sampleSphere);
	_sceneRenderer.SubmitModel(
		_cameraMesh,
		Matrix::CreateScale(0.4) * Matrix::CreateTranslation(_camera1.Data().Position)
	);
	_sceneRenderer.SubmitModel(
		_cameraMesh,
		Matrix::CreateScale(0.4) * Matrix::CreateTranslation(_camera2.Data().Position)
	);

	_sceneRenderer.EndSubmtions();

	_sceneRenderer.End();
}

void Scene::OnUi()
{
	ImGui::Begin("Scene");
	ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&_pointLight.Position), -2.0f, 15.0f);
	ImGui::SliderFloat3("Color", reinterpret_cast<float*>(&_pointLight.Color), 0.0f, 1.0f);
	ImGui::SliderFloat("Radius", &_pointLight.Radius, 0.1f, 10.0f);
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

	_sponzaScene->OnDebugUi();
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
