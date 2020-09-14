#pragma once

#include "Saffron/Renderer/RenderPass.h"

namespace Se
{
class OpenGLRenderPass : public RenderPass
{
public:
	explicit OpenGLRenderPass(const Specification &specification);
	virtual ~OpenGLRenderPass() = default;

	Specification &GetSpecification() override { return m_Specification; }
	const Specification &GetSpecification() const override { return m_Specification; }

private:
	Specification m_Specification;
};
}
