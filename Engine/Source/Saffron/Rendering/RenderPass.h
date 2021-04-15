#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Framebuffer.h"

namespace Se
{
struct RenderPassSpecification
{
	Shared<Framebuffer> TargetFramebuffer;
};

class RenderPass : public Managed
{
public:
	virtual ~RenderPass() = default;

	virtual RenderPassSpecification& GetSpecification() = 0;
	virtual const RenderPassSpecification& GetSpecification() const = 0;

	static Shared<RenderPass> Create(const RenderPassSpecification& specification);
};
}
