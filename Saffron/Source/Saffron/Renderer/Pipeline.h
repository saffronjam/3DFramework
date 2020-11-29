#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/VertexBuffer.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/Resource/Resource.h"

namespace Se
{
class Pipeline : public Resource
{
public:
	struct Specification
	{
		std::shared_ptr<Shader> Shader;
		VertexBuffer::Layout Layout;
	};

public:
	virtual ~Pipeline() = default;

	virtual Specification &GetSpecification() = 0;
	virtual const Specification &GetSpecification() const = 0;

	virtual void Invalidate() = 0;

	// TEMP: remove this when render command buffers are a thing
	virtual void Bind() = 0;

	static std::shared_ptr<Pipeline> Create(const Specification &spec);
};
}
