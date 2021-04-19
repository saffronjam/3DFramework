#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class BufferClearPass : public RenderPass
{
public:
	explicit BufferClearPass(String name);

	void Execute() override;

	void OnViewportResize(Uint32 width, Uint32 height) override;

private:
	Shared<Framebuffer> _buffer;
};
}
