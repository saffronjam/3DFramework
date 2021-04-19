#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
class CompositePass : public RenderPass
{
public:
	explicit CompositePass(String name);

	void Execute() override;

	void OnViewportResize(Uint32 width, Uint32 height) override;

private:
	Shared<Framebuffer> _finalFramebuffer;
	Shared<Framebuffer> _outputFramebuffer;
	Shared<Shader> _shader;
};
}
