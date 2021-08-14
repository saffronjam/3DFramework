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
	Output(std::string name, std::shared_ptr<class Framebuffer> framebuffer);

	auto Framebuffer() const -> const std::shared_ptr<class Framebuffer>&;
	void SetFramebuffer(const std::shared_ptr<class Framebuffer>& target);

	auto Name() const -> const std::string&;

private:
	std::string _name;
	
	std::shared_ptr<class Framebuffer> _framebuffer;
};
}
