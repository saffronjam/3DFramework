#include "SaffronPCH.h"


#include "Saffron/Rendering/RenderPasses/TargetHostPass.h"

namespace Se
{
TargetHostPass::TargetHostPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon),
	_target(Framebuffer::Create(FramebufferSpec{ 500, 500, {ImageFormat::RGBA, ImageFormat::Depth24Stencil8} }))
{
	RegisterOutput("Target", _target);
}

void TargetHostPass::Execute()
{
	_target->Bind();
	_target->Clear();
	_target->Unbind();
}
}
