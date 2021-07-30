#pragma once

#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class BufferClearPass : public RenderPass
{
public:
	explicit BufferClearPass(const std::string& name);

	void Execute() override;

private:
	std::shared_ptr<Framebuffer> _buffer;
};
}
