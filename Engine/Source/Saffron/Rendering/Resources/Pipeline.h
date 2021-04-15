#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resource.h"
#include "Saffron/Rendering/Resources/VertexBuffer.h"
#include "Saffron/Rendering/Resources/Shader.h"

namespace Se
{
struct PipelineSpecification
{
	Shared<Shader> Shader;
	VertexBufferLayout Layout;
};

class Pipeline : public Resource
{
public:
	virtual ~Pipeline() = default;

	virtual PipelineSpecification &GetSpecification() = 0;
	virtual const PipelineSpecification &GetSpecification() const = 0;

	virtual void Invalidate() = 0;

	// TEMP: remove this when render command buffers are a thing
	virtual void Bind() = 0;

	static Shared<Pipeline> Create(const PipelineSpecification &spec);
};
}
