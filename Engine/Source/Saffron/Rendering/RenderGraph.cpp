#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderGraph.h"

#include "Saffron/Rendering/RenderPasses/GeometryPass.h"

namespace Se
{
void RenderGraph::Setup(SceneCommon& sceneCommon)
{
	{
		auto pass = std::make_unique<GeometryPass>("geometry", sceneCommon);
		AddPass(std::move(pass));
	}

	// Final target
	{
		LinkGlobalInput("finalOutput", "geometry.target");
	}

	Connect();
}

void RenderGraph::Execute()
{
	for (auto& pass : _passes)
	{
		pass->Execute();
	}
}

void RenderGraph::SetViewportSize(uint width, uint height)
{
	for (auto& pass : _passes)
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
				"Failed to link global input with name {}. It did not exist. (Supplier: {})",
				fullInputName,
				provider
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

void RenderGraph::Connect()
{
	for (auto& pass : _passes)
	{
		ConnectInputs(pass->Inputs());
	}
	ConnectInputs(_globalInputs);
}

void RenderGraph::ConnectInputs(std::map<std::string, Input>& inputs)
{
	for (auto& input : inputs | std::views::values)
	{
		auto split = Utils::SplitString(input.Provider(), ".");

		if (split.size() != 2)
		{
			throw SaffronException(
				std::format(
					"Failed to connect input {} with provider {}. Provider was not separated with '.'",
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
					std::format("Failed to connect global input with name {}. It it did not exist.", split[1])
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
					std::format("Failed to connect to render pass with name {}. It it did not exist.", split[1])
				);
			}

			auto& renderPass = *renderPassFindResult;

			const auto outputFindResult = renderPass->Outputs().find(input.Provider());
			if (outputFindResult == renderPass->Outputs().end())
			{
				throw SaffronException(
					std::format(
						"Failed to connect {} --> {}. {} was not an input of {}.",
						input.Provider(),
						input.Name(),
						split[1],
						input.Name()
					)
				);
			}

			auto& output = outputFindResult->second;
			input.Connect(output);
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
