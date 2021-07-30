#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
RenderPass::RenderPass(std::string name) :
	_name(std::move(name))
{
}

auto RenderPass::Name() const -> const std::string&
{
	return _name;
}

auto RenderPass::Inputs() const -> const std::vector<Input>&
{
	return _inputs;
}

auto RenderPass::Outputs() const -> const std::vector<Output>&
{
	return _outputs;
}

void RenderPass::RegisterInput(std::string name, std::shared_ptr<Framebuffer>& framebuffer)
{
	_inputs.emplace_back(std::move(name), *this, framebuffer);
}

void RenderPass::RegisterOutput(std::string name, const std::shared_ptr<Framebuffer>& framebuffer)
{
	_outputs.emplace_back(std::move(name), *this, framebuffer);
}
}
