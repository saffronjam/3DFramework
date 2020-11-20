#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/VertexBuffer.h"
#include "Saffron/Renderer/Shader.h"

namespace Se
{
class Pipeline : public ReferenceCounted
{
public:
	struct Specification
	{
		Shared<Shader> Shader;
		VertexBuffer::Layout Layout;
	};

public:
	virtual ~Pipeline() = default;

	virtual Specification &GetSpecification() = 0;
	virtual const Specification &GetSpecification() const = 0;

	virtual void Invalidate() = 0;

	// TEMP: remove this when render command buffers are a thing
	virtual void Bind() = 0;

	static Shared<Pipeline> Create(const Specification &spec);
};
}
