#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Framebuffer.h"

namespace Se::Stream
{
class Output;

class Input
{
public:
	Input(String name, Shared<Framebuffer>& framebuffer);

	void Bind(const Output& output);

	const String& GetName() const;

	const String& GetDerivedPass() const;
	const String& GetDerivedPassOutput() const;
	void SetDerived(String derivedPass, String derivedPassOutput);

private:
	static void ValidateName(const String& name);

private:
	String _name;
	Shared<Framebuffer>& _framebuffer;

	String _derivedPass;
	String _derivedPassOutput;
};
}
