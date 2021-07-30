#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Stream/Input.h"
#include "Saffron/Rendering/Stream/Output.h"

namespace Se
{
class RenderPass
{
public:
	explicit RenderPass(std::string name);
	virtual ~RenderPass() = default;

	virtual void Execute() = 0;

	auto Name() const -> const std::string&;
	auto Inputs() const -> const std::vector<Input>&;
	auto Outputs() const -> const std::vector<Output>&;

protected:
	void RegisterInput(std::string name, std::shared_ptr<Framebuffer>& framebuffer);
	void RegisterOutput(std::string name, const std::shared_ptr<Framebuffer>& framebuffer);

private:
	std::string _name;
	std::vector<Input> _inputs;
	std::vector<Output> _outputs;
};
}
