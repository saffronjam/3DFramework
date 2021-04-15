#pragma once

#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class OpenGLRenderPass : public RenderPass
{
public:
	explicit OpenGLRenderPass(const RenderPassSpecification& spec);
	virtual ~OpenGLRenderPass() = default;

	RenderPassSpecification& GetSpecification() override;
	const RenderPassSpecification& GetSpecification() const override;

private:
	RenderPassSpecification m_Specification;
};
}
