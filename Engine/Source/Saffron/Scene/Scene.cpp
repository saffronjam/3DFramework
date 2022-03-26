#include "SaffronPCH.h"

#include "Saffron/Common/App.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Math/Random.h"
#include "Saffron/Scene/Entity.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/SceneRegistry.h"

namespace Se
{
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

	const auto& entityView = _registry.view<ModelComponent, TransformComponent>().each();

	_sceneRenderer.BeginSubmtions(RenderChannel_Geometry | RenderChannel_Shadow);

	for (auto&& [entity, model, transform] : entityView)
	{
		_sceneRenderer.SubmitModel(model.Model, transform.Transform());
	}

	_sceneRenderer.EndSubmtions();

	_sceneRenderer.BeginSubmtions(RenderChannel_Geometry);

	// Submit all model components
	for (auto&& [entity, model, transform] : entityView)
	{
		_sceneRenderer.SubmitModel(model.Model, transform.Transform());
	}


	// TEMPORARY
	_sceneRenderer.SubmitModel(
		_cameraMesh,
		Matrix::CreateScale(0.4) * Matrix::CreateTranslation(_camera1.Data().Position)
	);
	_sceneRenderer.SubmitModel(
		_cameraMesh,
		Matrix::CreateScale(0.4) * Matrix::CreateTranslation(_camera2.Data().Position)
	);
	_sceneRenderer.SubmitModel(
		_lightModel,
		Matrix::CreateScale(1.5) * Matrix::CreateTranslation(_sceneRenderer.SceneCommon().PointLight.Position)
	);
	////

	_sceneRenderer.EndSubmtions();

	_sceneRenderer.End();
}

void Scene::OnUi()
{
	ImGui::Begin("Scene");
	if (ImGui::Button("Milky way"))
	{
		_sceneRenderer.SceneCommon().Environment = SceneEnvironment::Create("Assets/Textures/Milkyway/Milkyway_BG.jpg");
	}
	if (ImGui::Button("Grand canyon"))
	{
		_sceneRenderer.SceneCommon().Environment = SceneEnvironment::Create(
			"Assets/Textures/GrandCanyon_C_YumaPoint/GCanyon_C_YumaPoint_3k.hdr"
		);
	}

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

auto Scene::HasSelectedEntity() const -> bool
{
	return SelectedEntity().Valid();
}

auto Scene::SelectedEntity() const -> Entity
{
	return Entity{_selectedEntityId, const_cast<Scene*>(this)};
}

void Scene::SetSelectedEntity(const Entity& entity)
{
	_selectedEntityId = entity.Id();
	OnSelcetedEntity.Invoke(entity);
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

auto Scene::Id() const -> Uuid
{
	return _id;
}

void Scene::SetViewportSize(uint width, uint height)
{
	_sceneRenderer.SetViewportSize(width, height);

	_camera1.SetProjection({Math::Pi / 4.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f});
	_camera2.SetProjection({Math::Pi / 4.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f});

	_viewportWidth = width;
	_viewportHeight = height;
}

auto Scene::CreateEntity(Uuid id) -> Entity
{
	return RegisterEntity(id);
}

auto Scene::CreateEntity(const std::string& name) -> Entity
{
	return CreateEntity(0, name);
}

auto Scene::CreateEntity(Uuid id, const std::string& name) -> Entity
{
	Entity entity = RegisterEntity(id);
	entity.Add<NameComponent>(name);
	entity.Add<TransformComponent>();
	return entity;
}

auto Scene::Registry() const -> const EntityRegistry&
{
	return const_cast<Scene&>(*this).Registry();
}

auto Scene::Registry() -> EntityRegistry&
{
	return _registry;
}

auto Scene::Create() -> const std::shared_ptr<Scene>&
{
	return SceneRegistry::CreateScene();
}

Scene::Scene() :
	_sceneRenderer(*this)
{
	auto entity = CreateEntity("My Entity");
	entity.Add<ModelComponent>(Model::Create("Sphere.fbx"));

	_pointLight.Position = Vector3{1.0f, 0.0f, 0.0f};
	_pointLight.Radius = 5.0f;

	_lightModel = Model::Create("Sphere.fbx");

	_cameraMesh = Model::Create("Sphere.fbx");

	_pointLight.Position = Vector3{0.0f, 5.0f, 0.0f};
	_pointLight.Radius = 4.0f;
	_pointLight.Color = Colors::White;

	Renderer().ViewportResized += [this](const SizeEvent& event)
	{
		ViewportResized.Invoke(event);
		return false;
	};
}

auto Scene::RegisterEntity(Uuid id) -> Entity
{
	if (id == Uuid::Null())
	{
		id = {};
	}
	id = static_cast<ulong>(_registry.create(static_cast<entt::entity>(static_cast<ulong>(id))));
	Entity entity{ id, this };
	entity.Add<IdComponent>(id);
	return entity;
}
}
