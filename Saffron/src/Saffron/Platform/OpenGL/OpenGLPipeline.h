#pragma once

#include "Saffron/Renderer/Pipeline.h"

namespace Se
{
class OpenGLPipeline : public Pipeline
{
public:
	OpenGLPipeline(Specification spec);
	virtual ~OpenGLPipeline();

	void Bind() override;
	void Invalidate() override;

	Specification &GetSpecification() override { return m_Specification; }
	const Specification &GetSpecification() const override { return m_Specification; }

private:
	Specification m_Specification;
	Uint32 m_VertexArrayRendererID = 0;
};
}