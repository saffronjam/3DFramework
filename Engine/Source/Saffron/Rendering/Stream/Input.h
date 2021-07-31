#pragma once

#include <string>
#include <memory>

#include "Saffron/Rendering/Bindables/Framebuffer.h"

namespace Se
{
class RenderPass;
class Output;

class Input
{
public:
	Input(std::string name, RenderPass& source, std::shared_ptr<Framebuffer>& framebufferRef);

	void Connect(const Output& output);
	void Connect(const std::shared_ptr<Framebuffer>& output);

	auto Name() const -> const std::string&;

private:
	std::string _name;
	RenderPass& _source;
	std::shared_ptr<Framebuffer>& _framebufferRef;
};
}
