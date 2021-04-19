#include "SaffronPCH.h"

#include "Saffron/Rendering/Stream/Output.h"

namespace Se::Stream
{
Output::Output(String name, Shared<Framebuffer>& framebuffer) :
	_name(Move(name)),
	_framebuffer(framebuffer)
{
}

const String& Output::GetName() const
{
	return _name;
}

const Shared<Framebuffer>& Output::GetFramebuffer() const
{
	return _framebuffer;
}
}
