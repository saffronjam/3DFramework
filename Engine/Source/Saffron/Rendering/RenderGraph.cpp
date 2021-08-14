#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderGraph.h"

#include "Saffron/Rendering/RenderPasses/GeometryPass.h"
#include "Saffron/Rendering/RenderPasses/LinePass.h"
#include "Saffron/Rendering/RenderPasses/ShadowMapPass.h"

namespace Se
{
void RenderGraph::OnUi()
{
	for (const auto& pass : _passes)
	{
		pass->OnUi();
	}
}

void RenderGraph::Setup(SceneCommon& sceneCommon)
{
	{
		auto pass = std::make_unique<ShadowMapPass>("ShadowMap", sceneCommon);
		AddPass(std::move(pass));
	}

	{
		auto pass = std::make_unique<GeometryPass>("Geometry", sceneCommon);
		pass->LinkInput("ShadowMap0", "ShadowMap.Target0");
		AddPass(std::move(pass));
	}

	{
		auto pass = std::make_unique<LinePass>("Lines", sceneCommon);
		pass->LinkInput("Target", "Geometry.Target");
		AddPass(std::move(pass));
	}

	// Final target
	{
		LinkGlobalInput("FinalOutput", "Lines.Target");
	}

	ConnectAll();

	for (const auto& pass : _passes)
	{
		pass->OnSetupFinished();
	}
}

void RenderGraph::Execute()
{
	for (const auto& pass : _passes)
	{
		pass->Execute();
	}
}

void RenderGraph::SetViewportSize(uint width, uint height)
{
	for (const auto& pass : _passes)
	{
		pass->SetViewportSize(width, height);
	}
}

void RenderGraph::RegisterInput(const std::string& name, std::shared_ptr<Framebuffer>& framebuffer)
{
	std::ostringstream oss;
	oss << "$." << name;
	_globalInputs.emplace(oss.str(), Input{oss.str(), framebuffer});
}

void RenderGraph::RegisterOutput(const std::string& name, const std::shared_ptr<Framebuffer>& framebuffer)
{
	std::ostringstream oss;
	oss << "$." << name;
	_globalOutputs.emplace(oss.str(), Output{oss.str(), framebuffer});
}

void RenderGraph::LinkGlobalInput(const std::string& input, const std::string& provider)
{
	const auto fullInputName = "$." + input;

	const auto findResult = _globalInputs.find(fullInputName);
	if (findResult == _globalInputs.end())
	{
		throw SaffronException(
			std::format(
				"Failed to globally link '{}' to '{}' because '{}' was not registered as a global input",
				provider,
				fullInputName,
				input
			)
		);
	}

	auto& inputResult = findResult->second;
	inputResult.SetProvider(provider);
}

void RenderGraph::AddPass(std::unique_ptr<RenderPass> pass)
{
	_passes.emplace_back(std::move(pass));
}

void RenderGraph::ConnectAll()
{
	for (const auto& pass : _passes)
	{
		Connect(pass->Name(), pass->Inputs(), pass->Outputs());
	}
	Connect("$", _globalInputs, _globalOutputs);
}

void RenderGraph::Connect(
	const std::string& passName,
	std::map<std::string, Input>& inputs,
	std::map<std::string, Output>& outputs
)
{
	for (auto& input : inputs | std::views::values)
	{
		auto split = Utils::SplitString(input.Provider(), ".");

		if (split.size() != 2)
		{
			throw SaffronException(
				std::format(
					"Failed to connect '{}' to '{}' because '{}' was not separated with '.'",
					input.Provider(),
					input.Name(),
					input.Provider()
				)
			);
		}

		// Check if global provider
		if (split[0] == "$")
		{
			const auto findResult = _globalOutputs.find(split[1]);
			if (findResult == _globalOutputs.end())
			{
				throw SaffronException(
					std::format(
						"Failed to globally connect '{}' to '{}' because '{}' was not registered as a global input",
						input.Provider(),
						input.Name(),
						split[1]
					)
				);
			}

			auto& output = findResult->second;
			input.Connect(output);
		}
		else
		{
			// Find the pass that provides the target
			const auto renderPassFindResult = std::ranges::find_if(
				_passes,
				[split](const std::unique_ptr<RenderPass>& renderPass)
				{
					return renderPass->Name() == split[0];
				}
			);

			if (renderPassFindResult == _passes.end())
			{
				throw SaffronException(
					std::format(
						"Failed to connect '{}' to '{}' because '{}' was not registered as a render pass.",
						input.Provider(),
						input.Name(),
						split[0]
					)
				);
			}

			const auto& renderPass = *renderPassFindResult;

			const auto outputFindResult = renderPass->Outputs().find(input.Provider());
			if (outputFindResult == renderPass->Outputs().end())
			{
				throw SaffronException(
					std::format(
						"Failed to connect '{}' to '{}' because '{}' was not registered as input to '{}'.",
						input.Provider(),
						input.Name(),
						split[1],
						split[0]
					)
				);
			}

			auto& output = outputFindResult->second;
			input.Connect(output);
		}

		// Check if there is any streams connected within the pass
		// Eg. input "Target" and output "Target" should be linked internally
		for (auto& [name, input] : inputs)
		{
			const auto interalFindResult = outputs.find(name);
			if (interalFindResult != outputs.end())
			{
				interalFindResult->second.SetFramebuffer(input.Framebuffer());
			}
		}
	}
}

namespace Utils
{
template <class Iter>
void SplitStringIter(const std::string& s, const std::string& delim, Iter out)
{
	if (delim.empty())
	{
		*out++ = s;
	}
	else
	{
		size_t a = 0, b = s.find(delim);
		for (; b != std::string::npos; a = b + delim.length(), b = s.find(delim, a))
		{
			*out++ = std::move(s.substr(a, b - a));
		}
		*out++ = std::move(s.substr(a, s.length() - a));
	}
}

std::vector<std::string> SplitString(const std::string& input, const std::string& delim)
{
	std::vector<std::string> strings;
	SplitStringIter(input, delim, std::back_inserter(strings));
	return strings;
}
}
}
