#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/EditorCamera.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/ShaderStructs.h"
#include "Saffron/Rendering/Bindables/ConstantBuffer.h"
#include "Saffron/Rendering/SceneEnvironment.h"

namespace Se
{
class Scene
{
public:
	Scene();

	void OnUpdate(TimeSpan ts);
	void OnRender();
	void OnUi();

	auto Renderer() const -> const SceneRenderer&;

	// Will use entities later
	auto SelectedModel() -> std::shared_ptr<Model>&;
	auto SelectedModel() const -> const std::shared_ptr<Model>&;

	auto Camera() -> EditorCamera&;
	auto Camera() const -> const EditorCamera&;

	auto ViewportWidth() const -> uint;
	auto ViewportHeight() const -> uint;

	void SetViewportSize(uint width, uint height);

public:
	mutable SubscriberList<const SizeEvent&> ViewportResized;

private:
	int _activeRadioButton = 0;
	EditorCamera _camera1;
	EditorCamera _camera2;
	std::shared_ptr<Model> _cameraMesh;
	EditorCamera* _activeCamera = &_camera1;
	bool _drawCameraFrustums = false;
	std::shared_ptr<SceneEnvironment> _environment;

	SceneRenderer _sceneRenderer;
	PointLight _pointLight;

	uint _viewportWidth = 0, _viewportHeight = 0;

	std::vector<std::shared_ptr<Model>> _models;
	float metalness = 0.0f;
	float roughness = 0.2f;
	Vector3 albedo = {1.0f, 0.0f, 0.0f};


	std::shared_ptr<Texture> _testWicTex;

};
}
