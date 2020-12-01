#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/Framebuffer.h"

namespace Se
{
class RenderPass : public Resource
{
public:
	struct Specification
	{
		std::shared_ptr<Framebuffer> TargetFramebuffer;
	};

public:
	virtual ~RenderPass() = default;

	virtual Specification &GetSpecification() = 0;
	virtual const Specification &GetSpecification() const = 0;

	static std::shared_ptr<RenderPass> Create(const Specification &specification);

};
}

