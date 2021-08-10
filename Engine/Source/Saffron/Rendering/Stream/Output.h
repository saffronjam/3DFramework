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
	Output(std::string name, std::shared_ptr<Framebuffer> framebuffer);

	auto Target() const -> const std::shared_ptr<Framebuffer>&;

	auto Name() const -> const std::string&;

private:
	std::string _name;
	
	std::shared_ptr<Framebuffer> _framebuffer;
};
}
