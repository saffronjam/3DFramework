#include "SaffronPCH.h"

#include "Saffron/Rendering/Stream/Input.h"

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Stream/Output.h"

namespace Se
{
Input::Input(std::string name, RenderPass& source, std::shared_ptr<Framebuffer>& framebufferRef) :
	_name(std::move(name)),
	_source(source),
	_framebufferRef(framebufferRef)
{
}

void Input::Connect(const Output& output)
{
	_framebufferRef = output.Target();
}

auto Input::Name() const -> const std::string&
{
	return _name;
}
}
