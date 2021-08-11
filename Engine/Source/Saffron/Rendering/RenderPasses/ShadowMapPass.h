#pragma once

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Bindables/MvpCBuffer.h"
#include "Saffron/Rendering/Bindables/Shader.h"

namespace Se
{
class ShadowMapPass : public RenderPass
{
public:
	explicit ShadowMapPass(const std::string& name, struct SceneCommon& sceneCommon);

	void OnUi() override;
	
	void Execute() override;

private:
	std::shared_ptr<Framebuffer> _shadowMap;
	std::shared_ptr<Shader> _shadowShader;
	std::shared_ptr<Shader> _depthTextureShader;
	std::shared_ptr<MvpCBuffer> _mvpCBuffer;

	static constexpr uint Width = 1024, Height = 1024;
};
}
