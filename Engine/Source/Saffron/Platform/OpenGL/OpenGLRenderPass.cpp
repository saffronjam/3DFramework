#include "SaffronPCH.h"
#include "OpenGLRenderPass.h"

namespace Se
{
OpenGLRenderPass::OpenGLRenderPass(const RenderPassSpecification& spec) :
	_specification(spec)
{
}

RenderPassSpecification& OpenGLRenderPass::GetSpecification()
{
	return _specification;
}

const RenderPassSpecification& OpenGLRenderPass::GetSpecification() const
{
	return _specification;
}
}
