#pragma once

#include "Saffron/Rendering/Resources/Pipeline.h"

namespace Se
{
class OpenGLPipeline : public Pipeline
{
public:
	explicit OpenGLPipeline(const PipelineSpecification& specification);
	virtual ~OpenGLPipeline();

	void Bind() override;
	void Invalidate() override;

	PipelineSpecification& GetSpecification() override;
	const PipelineSpecification& GetSpecification() const override;

private:
	PipelineSpecification m_Specification;
	uint32_t m_VertexArrayRendererID = 0;
};
}
