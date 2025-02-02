﻿#pragma once

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Rendering/PointLight.h"

namespace Se
{
struct alignas(16) PointLightCBuffer
{
	std::array<ShaderStructs::PointLight, MaxLights> PointLights;
	int nPointLights;
	Color Ambient;
};

class GeometryPass : public RenderPass
{
public:
	explicit GeometryPass(const std::string& name, struct SceneCommon& sceneCommon);

	void OnSetupFinished() override;
	void OnUi() override;

	void Execute() override;

	void SetViewportSize(uint width, uint height) override;

private:
	std::shared_ptr<Framebuffer> _target;
	std::shared_ptr<Framebuffer> _shadowMap;
	std::shared_ptr<ConstantBuffer<PointLightCBuffer>> _pointLightCBuffer;
	std::shared_ptr<TransformCBuffer> _mvpCBuffer;

	std::shared_ptr<Texture> _shadowMapTexture;

};
}
