#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
RenderPass::RenderPass(std::string name) :
	_name(std::move(name))
{
}

void RenderPass::Connect(const Output& output, const std::string& inside)
{
	auto result = _inputs.find(inside);

	if (result == _inputs.end())
	{
		Log::Debug("Render pass {} had no input with name: {}", _name, inside);
		return;
	}

	auto& input = result->second;
	input.Connect(output);
}

void RenderPass::Connect(const std::shared_ptr<Framebuffer>& output, const std::string& inside)
{
	auto result = _inputs.find(inside);

	if (result == _inputs.end())
	{
		Log::Debug("Render pass {} had no input with name: {}", _name, inside);
		return;
	}

	auto& input = result->second;
	input.Connect(output);
}

auto RenderPass::Name() const -> const std::string&
{
	return _name;
}

auto RenderPass::Inputs() const -> const std::map<std::string, Input>&
{
	return _inputs;
}

auto RenderPass::Outputs() const -> const std::map<std::string, Output>&
{
	return _outputs;
}

void RenderPass::RegisterInput(const std::string& name, std::shared_ptr<Framebuffer>& framebuffer)
{
	_inputs.emplace(name, Input{std::move(name), *this, framebuffer});
}

void RenderPass::RegisterOutput(const std::string& name, const std::shared_ptr<Framebuffer>& framebuffer)
{
	_outputs.emplace(name, Output{std::move(name), *this, framebuffer});
}
}
