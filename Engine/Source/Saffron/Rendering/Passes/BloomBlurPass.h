#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
class BloomBlurPass : public RenderPass
{
public:
	explicit BloomBlurPass(String name, int iterations = 10);

	void Execute() override;

	void OnViewportResize(uint width, uint height) override;

private:
	inline const Shared<Framebuffer>& HorizOrVert(int index);

private:
	const int _iterations;

	Shared<Framebuffer> _inputFb;
	Shared<Framebuffer> _verticalFb;
	Shared<Framebuffer> _horiontalFb;
	Shared<Framebuffer> _outputFb;

	Shared<Shader> _bloomBlurShader;
	Shared<Shader> _bloomBlendShader;
};
}
