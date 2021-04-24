#include "SaffronPCH.h"

#include "Saffron/Rendering/Stream/Input.h"
#include "Saffron/Rendering/Stream/Output.h"

namespace Se::Stream
{
Input::Input(String name, Shared<Framebuffer>& framebuffer) :
	_name(Move(name)),
	_framebuffer(framebuffer)
{
	ValidateName(_name);
}

void Input::Bind(const Output& output)
{
	_framebuffer = output.GetFramebuffer();
}

const String& Input::GetName() const
{
	return _name;
}

const String& Input::GetDerivedPass() const
{
	return _derivedPass;
}

const String& Input::GetDerivedPassOutput() const
{
	return _derivedPassOutput;
}

void Input::SetDerived(String derivedPass, String derivedPassOutput)
{
	if (derivedPass != "$")
	{
		ValidateName(derivedPass);
	}
	ValidateName(derivedPassOutput);

	_derivedPass = Move(derivedPass);
	_derivedPassOutput = Move(derivedPassOutput);
}

void Input::ValidateName(const String& name)
{
	Debug::Assert(!name.empty(), "Invalid pass- or output name in stream system: <Empty>");
	const bool validName = std::all_of(name.begin(), name.end(), [](char c)
	{
		return std::isalnum(c) || c == '_';
	});
	Debug::Assert(validName && !std::isdigit(name.front()), "Invalid pass- or output name in stream system: " + name);;
}
}
