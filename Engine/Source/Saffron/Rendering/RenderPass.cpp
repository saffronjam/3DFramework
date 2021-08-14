#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPass.h"

#include "SceneRenderer.h"

namespace Se
{
RenderPass::RenderPass(std::string name, struct SceneCommon& sceneCommon) :
	_name(std::move(name)),
	_sceneCommon(sceneCommon)
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

auto RenderPass::Inputs() -> std::map<std::string, Input>&
{
	return _inputs;
}

auto RenderPass::Inputs() const -> const std::map<std::string, Input>&
{
	return _inputs;
}

auto RenderPass::Outputs() -> std::map<std::string, Output>&
{
	return _outputs;
}

auto RenderPass::Outputs() const -> const std::map<std::string, Output>&
{
	return _outputs;
}

void RenderPass::LinkInput(const std::string& input, const std::string& provider)
{
	std::ostringstream oss;
	oss << _name << '.' << input;
	const auto fullInputName = oss.str();

	const auto findResult = _inputs.find(fullInputName);
	if (findResult == _inputs.end())
	{
		throw SaffronException(
			std::format(
				"Failed to link '{}' to '{}' because '{}' was not registered as an input to '{}'",
				provider,
				fullInputName,
				input,
				_name
			)
		);
	}

	auto& inputResult = findResult->second;
	inputResult.SetProvider(provider);
}

void RenderPass::RegisterInput(const std::string& name, std::shared_ptr<Framebuffer>& framebuffer)
{
	std::ostringstream oss;
	oss << _name << '.' << name;
	_inputs.emplace(oss.str(), Input{oss.str(), framebuffer});
}

void RenderPass::RegisterOutput(const std::string& name, const std::shared_ptr<Framebuffer>& framebuffer)
{
	std::ostringstream oss;
	oss << _name << '.' << name;
	_outputs.emplace(oss.str(), Output{oss.str(), framebuffer});
}

auto RenderPass::SceneCommon() -> Se::SceneCommon&
{
	return _sceneCommon;
}

auto RenderPass::SceneCommon() const -> const Se::SceneCommon&
{
	return const_cast<RenderPass&>(*this).SceneCommon();
}
}
