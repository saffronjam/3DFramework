#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Stream/Output.h"

namespace Se
{
Output::Output(std::string name, std::shared_ptr<class Framebuffer> framebuffer) :
	_name(std::move(name)),
	_framebuffer(framebuffer)
{
}

auto Output::Framebuffer() const -> const std::shared_ptr<class Framebuffer>&
{
	return _framebuffer;
}

void Output::SetFramebuffer(const std::shared_ptr<class Framebuffer>& target)
{
	_framebuffer = target;
}

auto Output::Name() const -> const std::string&
{
	return _name;
}
}
