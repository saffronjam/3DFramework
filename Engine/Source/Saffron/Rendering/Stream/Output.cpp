#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Stream/Output.h"

namespace Se
{
Output::Output(std::string name, std::shared_ptr<Framebuffer> framebuffer) :
	_name(std::move(name)),
	_framebuffer(framebuffer)
{
}

auto Output::Target() const -> const std::shared_ptr<Framebuffer>&
{
	return _framebuffer;
}

auto Output::Name() const -> const std::string&
{
	return _name;
}
}
