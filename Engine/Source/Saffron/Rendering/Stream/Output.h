#pragma once

#include <string>
#include <memory>

#include "Saffron/Rendering/Bindables/Framebuffer.h"

namespace Se
{
class RenderPass;

class Output
{
public:
	Output(std::string name, RenderPass& source, std::shared_ptr<Framebuffer> framebuffer);

	auto Target() const -> const std::shared_ptr<Framebuffer>&;

private:
	std::string _name;
	RenderPass& _source;
	std::shared_ptr<Framebuffer> _framebuffer;
};
}
