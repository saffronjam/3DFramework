#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/Framebuffer.h"

namespace Se
{
class RenderPass : public ReferenceCounted
{
public:
	struct Specification
	{
		Shared<Framebuffer> TargetFramebuffer;
	};

public:
	virtual ~RenderPass() = default;

	virtual Specification &GetSpecification() = 0;
	virtual const Specification &GetSpecification() const = 0;

	static Shared<RenderPass> Create(const Specification &specification);

};
}

