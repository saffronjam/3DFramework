#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/VertexBuffer.h"
#include "Saffron/Renderer/Shader.h"

namespace Se
{
class Pipeline : public RefCounted
{
public:
	struct Specification
	{
		Ref<Shader> Shader;
		VertexBuffer::Layout Layout;
	};

public:
	virtual ~Pipeline() = default;

	virtual Specification &GetSpecification() = 0;
	virtual const Specification &GetSpecification() const = 0;

	virtual void Invalidate() = 0;

	// TEMP: remove this when render command buffers are a thing
	virtual void Bind() = 0;

	static Ref<Pipeline> Create(const Specification &spec);
};
}
