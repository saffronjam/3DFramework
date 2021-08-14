#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/EditorCamera.h"
#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/ShaderStructs.h"
#include "Saffron/Rendering/Bindables/ConstantBuffer.h"

namespace Se
{
struct alignas(16) CommonCBuffer
{
	Vector3 CameraPosition;
};

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
	std::shared_ptr<Mesh> _cameraMesh;
	EditorCamera* _activeCamera = &_camera1;
	bool _drawCameraFrustums;


	SceneRenderer _sceneRenderer;
	std::shared_ptr<ConstantBuffer<CommonCBuffer>> _commonCBuffer;
	PointLight _pointLight;

	uint _viewportWidth = 0, _viewportHeight = 0;

	std::vector<std::shared_ptr<Mesh>> _sampleMeshes;
	std::vector<Vector3> _sampleMeshesPosition;
	std::shared_ptr<Mesh> _sampleSphere;
	std::shared_ptr<Mesh> _samplePlane;
};
}
