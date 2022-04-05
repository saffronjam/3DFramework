#pragma once

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/PointLight.h"

namespace Se
{
struct alignas(16) ShadowMapPassCBuffer
{
	Color Values;
	Matrix Model;
	Matrix Mvp;
	Vector3 Position;
	float Radius;
};

class ShadowMapPass : public RenderPass
{
public:
	explicit ShadowMapPass(const std::string& name, struct SceneCommon& sceneCommon);

	void OnSetupFinished() override;

	void Execute() override;

private:
	const std::shared_ptr<Framebuffer> _shadowMap;

	bool _orthographic = false;
};
}
