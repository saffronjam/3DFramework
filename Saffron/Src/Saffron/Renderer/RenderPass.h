#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/Framebuffer.h"

namespace Se
{
class RenderPass : public RefCounted
{
public:
	struct Specification
	{
		Ref<Framebuffer> TargetFramebuffer;
	};

public:
	virtual ~RenderPass() = default;

	virtual Specification &GetSpecification() = 0;
	virtual const Specification &GetSpecification() const = 0;

	static Ref<RenderPass> Create(const Specification &specification);

};
}

