#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/EditorCamera.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/PointLight.h"
#include "Saffron/Rendering/Bindables/ConstantBuffer.h"
#include "Saffron/Rendering/SceneEnvironment.h"
#include "Saffron/Scene/EntityRegistry.h"
#include "Saffron/Scene/EntityComponents.h"

namespace Se
{
class Entity;

class Scene : public Managed<Scene>
{
public:
	void OnUpdate(TimeSpan ts);
	void OnRender();
	void OnUi();

	auto Renderer() const -> const SceneRenderer&;

	auto HasSelectedEntity() const -> bool;
	auto SelectedEntity() const -> Entity;
	void SetSelectedEntity(const Entity& entity);

	auto Camera() -> EditorCamera&;
	auto Camera() const -> const EditorCamera&;

	auto ViewportWidth() const -> uint;
	auto ViewportHeight() const -> uint;

	auto Id() const -> Uuid;

	void SetViewportSize(uint width, uint height);

	auto CreateEntity(Uuid id) -> Entity;
	auto CreateEntity(const std::string& name) -> Entity;
	auto CreateEntity(Uuid id, const std::string& name) -> Entity;
	auto Registry() const -> const EntityRegistry&;
	auto Registry() -> EntityRegistry&;

	static auto Create() -> const std::shared_ptr<Scene>&;

public:
	SubscriberList<const Entity&> OnSelcetedEntity;

private:
	Scene();
	auto RegisterEntity(Uuid id) -> Entity;

public:
	mutable SubscriberList<const SizeEvent&> ViewportResized;

private:
	Uuid _id{};

	Uuid _selectedEntityId = Uuid::Null();

	int _activeRadioButton = 0;
	EditorCamera _camera1;
	EditorCamera _camera2;
	std::shared_ptr<Model> _cameraMesh;
	EditorCamera* _activeCamera = &_camera1;
	bool _drawCameraFrustums = false;

	SceneRenderer _sceneRenderer;
	PointLight _pointLight;

	uint _viewportWidth = 0, _viewportHeight = 0;

	std::shared_ptr<Model> _lightModel;
	float metalness = 0.0f;
	float roughness = 0.2f;
	Vector3 albedo = {1.0f, 0.0f, 0.0f};

	// Entities
	EntityRegistry _registry;

	friend class SceneRegistry;
};
}
