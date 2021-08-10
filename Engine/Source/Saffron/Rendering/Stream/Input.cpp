#include "SaffronPCH.h"

#include "Saffron/Rendering/Stream/Input.h"

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Stream/Output.h"

namespace Se
{
Input::Input(std::string name, std::shared_ptr<Framebuffer>& framebufferRef) :
	_name(std::move(name)),
	_framebufferRef(framebufferRef)
{
}

void Input::Connect(const Output& output)
{
	_framebufferRef = output.Target();
}

void Input::Connect(const std::shared_ptr<Framebuffer>& output)
{
	_framebufferRef = output;
}

auto Input::Name() const -> const std::string&
{
	return _name;
}

auto Input::Provider() const -> const std::string&
{
	return _provider;
}

void Input::SetProvider(const std::string& provider)
{
	_provider = provider;
}
}
