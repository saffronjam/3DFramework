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
	Input(std::string name, std::shared_ptr<class Framebuffer>& framebufferRef);

	void Connect(const Output& output);
	void Connect(const std::shared_ptr<class Framebuffer>& output);

	auto Name() const -> const std::string&;
	auto Provider() const -> const std::string&;

	void SetProvider(const std::string& provider);

	auto Framebuffer() -> const std::shared_ptr<class Framebuffer>&;

private:
	std::string _name;
	std::string _provider;

	std::shared_ptr<class Framebuffer>& _framebufferRef;
};
}
