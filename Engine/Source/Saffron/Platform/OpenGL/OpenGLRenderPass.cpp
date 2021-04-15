#include "SaffronPCH.h"
#include "OpenGLRenderPass.h"

namespace Se
{
OpenGLRenderPass::OpenGLRenderPass(const RenderPassSpecification& spec) :
	m_Specification(spec)
{
}

RenderPassSpecification& OpenGLRenderPass::GetSpecification()
{
	return m_Specification;
}

const RenderPassSpecification& OpenGLRenderPass::GetSpecification() const
{
	return m_Specification;
}
}
