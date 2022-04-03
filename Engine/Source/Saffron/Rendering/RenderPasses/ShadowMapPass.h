#pragma once

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/PointLight.h"
#include "Saffron/Rendering/Bindables/TransformCBuffer.h"
#include "Saffron/Rendering/Bindables/Shader.h"

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
	void OnUi() override;

	void Execute() override;

private:
	const std::shared_ptr<Framebuffer>& _shadowMap;
	std::shared_ptr<Shader> _shadowShader;
	std::shared_ptr<Shader> _depthTextureShader;

	std::shared_ptr<ConstantBuffer<ShadowMapPassCBuffer>> _shadowMapCBuffer;

	bool _orthographic = false;
};
}
