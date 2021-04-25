#pragma once

#include "Saffron/Rendering/Resources/Pipeline.h"

namespace Se
{
class OpenGLPipeline : public Pipeline
{
public:
	explicit OpenGLPipeline(PipelineSpecification specification);
	virtual ~OpenGLPipeline();

	void Bind() override;
	void Invalidate() override;

	PipelineSpecification& GetSpecification() override;
	const PipelineSpecification& GetSpecification() const override;

private:
	PipelineSpecification _specification;
	uint _vertexArrayRendererID = 0;
};
}
