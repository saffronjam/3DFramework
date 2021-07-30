#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Stream/Output.h"

namespace Se
{
Output::Output(std::string name, RenderPass& source, std::shared_ptr<Framebuffer> framebuffer) :
	_name(std::move(name)),
	_source(source),
	_framebuffer(framebuffer)
{
}

auto Output::Target() const -> const std::shared_ptr<Framebuffer>&
{
	return _framebuffer;
}
}
