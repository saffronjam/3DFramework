#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Framebuffer.h"

namespace Se::Stream
{
class Output
{
public:
	Output(String name, Shared<Framebuffer> &framebuffer);

	const String& GetName() const;

	const Shared<Framebuffer> &GetFramebuffer() const;

private:
	String _name;
	Shared<Framebuffer>& _framebuffer;

	String _derivedPass;
	String _derivedPassOutput;
};
}
