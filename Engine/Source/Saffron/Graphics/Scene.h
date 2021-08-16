#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/EditorCamera.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/ShaderStructs.h"
#include "Saffron/Rendering/Bindables/ConstantBuffer.h"

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
	
	SceneRenderer _sceneRenderer;
	PointLight _pointLight;

	uint _viewportWidth = 0, _viewportHeight = 0;
	
	std::shared_ptr<Model> _sampleSphere;
	std::shared_ptr<Model> _sponzaScene;
};
}
