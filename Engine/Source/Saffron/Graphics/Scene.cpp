#include "SaffronPCH.h"

#include "Saffron/Graphics/Scene.h"

#include "Saffron/Common/App.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Math/Random.h"

namespace Se
{
Scene::Scene() :
	_sceneRenderer(*this)
{
	_pointLight.Position = Vector3{ 1.0f, 0.0f, 0.0f };
	_pointLight.Radius = 5.0f;

	_models.push_back(Model::Create("Cube.fbx"));
	_models.back()->Transform() = Matrix::CreateScale(25.0f, 0.5f, 12.0f) * Matrix::CreateTranslation(
		0.0f,
		-3.0f,
		0.0f
	);

	_lightModel = Model::Create("Sphere.fbx");

	constexpr int nBalls = 3;
	for (int i = 0; i < nBalls; i++)
	{
		const float progress = static_cast<float>(i) / static_cast<float>(nBalls - 1);

		const auto pos = Random::Vec3(-1, -1, -1, 1, 1, 1) * 20.0f;

		_models.push_back(Model::Create("Sphere.fbx"));
		//_models.back()->Transform() = Matrix::CreateTranslation(-5.0f + static_cast<float>(i) * 2.5f, 4, 0.0f);
		//_models.back()->Transform() = Matrix::CreateTranslation(pos);
		_models.back()->Transform() = Matrix::CreateTranslation(0.0f, static_cast<float>(-nBalls) / 2 + i, 0.0f);
		_models.back()->Materials().front()->SetMaterialData(
			MaterialDataCBuf{Colors::Red, 0.0f, 0.00f, 1.0f, 0.0f, false}
		);
	}

	_cameraMesh = Model::Create("Sphere.fbx");

	_pointLight.Position = Vector3{0.0f, 5.0f, 0.0f};
	_pointLight.Radius = 4.0f;
	_pointLight.Color = Colors::White;

	Renderer().ViewportResized += [this](const SizeEvent& event)
	{
		ViewportResized.Invoke(event);
		return false;
	};

	cube = TextureCube::Create(
		1024,
		1024,
		ImageFormat::RGBA32f,
		{.CreateSampler = false, .Usage = TextureUsage_ShaderResource | TextureUsage_UnorderedAccess}
	);
}

void Scene::OnUpdate(TimeSpan ts)
{
	_activeCamera->OnUpdate(ts);
}

void Scene::OnRender()
{
	_sceneRenderer.SceneCommon().PointLight = _pointLight;


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

	for (const auto& model : _models)
	{
		_sceneRenderer.SubmitModel(model);
	}

	_sceneRenderer.EndSubmtions();

	_sceneRenderer.BeginSubmtions(RenderChannel_Geometry);

	for (const auto& model : _models)
	{
		_sceneRenderer.SubmitModel(model);
	}
	_sceneRenderer.SubmitModel(
		_cameraMesh,
		Matrix::CreateScale(0.4) * Matrix::CreateTranslation(_camera1.Data().Position)
	);
	_sceneRenderer.SubmitModel(
		_cameraMesh,
		Matrix::CreateScale(0.4) * Matrix::CreateTranslation(_camera2.Data().Position)
	);
	_sceneRenderer.SubmitModel(_lightModel, Matrix::CreateScale(1.5) * Matrix::CreateTranslation(_sceneRenderer.SceneCommon().PointLight.Position));

	_sceneRenderer.EndSubmtions();

	_sceneRenderer.End();
}

void Scene::OnUi()
{
	ImGui::Begin("Scene");
	if (ImGui::Button("Milky way"))
	{
		_sceneRenderer.SceneCommon().Environment = SceneEnvironment::Create(
			"Assets/Textures/Milkyway/Milkyway_BG.jpg"
		);
	}
	if (ImGui::Button("Grand canyon"))
	{
		_sceneRenderer.SceneCommon().Environment = SceneEnvironment::Create(
			"Assets/Textures/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr"
		);
	}


	if (ImGui::SliderFloat("Metalness", &metalness, 0.0f, 1.0f))
	{
		for (auto& model : _models)
		{
			auto& cb = model->Materials().front()->CBuffer();
			auto& data = cb.Data();
			data.Metalness = metalness;
			cb.UploadData();
		}
	}
	if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
	{
		for (auto& model : _models)
		{
			auto& cb = model->Materials().front()->CBuffer();
			auto& data = cb.Data();
			data.Roughness = roughness;
			cb.UploadData();
		}
	}
	if (ImGui::SliderFloat3("Albedo", reinterpret_cast<float*>(&albedo), 0.0f, 1.0f))
	{
		for (auto& model : _models)
		{
			auto& cb = model->Materials().front()->CBuffer();
			auto& data = cb.Data();
			data.AlbedoColor = Color(albedo.x, albedo.y, albedo.z);
			cb.UploadData();
		}
	}

	_models[1]->OnDebugUi();

	ImGui::Separator();
	ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&_pointLight.Position), -25.0f, 25.0f);
	ImGui::SliderFloat3("Color", reinterpret_cast<float*>(&_pointLight.Color), 0.0f, 1.0f);
	ImGui::SliderFloat("Radius", &_pointLight.Radius, 0.1f, 50.0f);
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

auto Scene::SelectedModel() -> std::shared_ptr<Model>&
{
	return _models.front();
}

auto Scene::SelectedModel() const -> const std::shared_ptr<Model>&
{
	return _models.front();
}

auto Scene::Camera() -> EditorCamera&
{
	return *_activeCamera;
}

auto Scene::Camera() const -> const EditorCamera&
{
	return *_activeCamera;
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
