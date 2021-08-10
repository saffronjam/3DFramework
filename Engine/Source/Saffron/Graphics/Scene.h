#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/ShaderStructs.h"
#include "Saffron/Rendering/Bindables/ConstantBuffer.h"

namespace Se
{
static constexpr uint MaxLights = 4;

struct alignas(16) PointLightCBuffer
{
	PointLight PointLights[MaxLights];
	int nPointLights;
};

class Scene
{
public:
	Scene();

	void OnUpdate();
	void OnRender(const struct CameraData& cameraData);

	auto Renderer() const -> const SceneRenderer&;
	
	void SetViewportSize(uint width, uint height);

public:
	mutable SubscriberList<const SizeEvent&> ViewportResized;

private:
	SceneRenderer _sceneRenderer;
	ConstantBuffer<PointLightCBuffer> _pointLightCBuffer;

	std::shared_ptr<Mesh> _sampleMesh;
	
};
}
