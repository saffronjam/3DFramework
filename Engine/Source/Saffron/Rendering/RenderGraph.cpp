#include "SaffronPCH.h"

#include "Saffron/Core/GenUtils.h"
#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
RenderGraph::RenderGraph()
{
	AddGlobalInput(CreateUnique<Stream::Input>("backBuffer", _backBuffer));
}

void RenderGraph::OnViewportResize(uint width, uint height)
{
	for (auto& pass : _passes)
	{
		pass->OnViewportResize(width, height);
	}
}

void RenderGraph::AddPass(Unique<RenderPass> pass)
{
	Debug::Assert(std::none_of(_passes.begin(), _passes.end(), [&pass](const Unique<RenderPass>& elem)
	{
		return pass->GetName() == elem->GetName();
	}), "Render pass was already added: {0}", pass);
	LinkInputs(*pass);
	_passes.push_back(Move(pass));
}

void RenderGraph::AddInput(const Stream::Input& input)
{
	_inputs.push_back(input);
}

void RenderGraph::AddOutput(const Stream::Output& output)
{
	_outputs.push_back(output);
}

void RenderGraph::AddGlobalInput(Unique<Stream::Input> input)
{
	_globalInputs.push_back(Move(input));
}

void RenderGraph::AddGlobalOutput(Unique<Stream::Output> output)
{
	_globalOutputs.push_back(Move(output));
}

void RenderGraph::SetGlobalStreamLinkage(const String& outputName, const String& inputName)
{
	const auto result = std::find_if(_globalInputs.begin(), _globalInputs.end(),
	                                 [&inputName](const Unique<Stream::Input>& sink)
	                                 {
		                                 return sink->GetName() == inputName;
	                                 });

	Debug::Assert(result != _globalInputs.end(), ("Global sink does not exist: " + inputName).c_str());;

	auto targetSplit = GenUtils::SplitString(outputName, ".");
	Debug::Assert(targetSplit.size() >= 2u, "Input target has incorrect format");

	(*result)->SetDerived(targetSplit[0], targetSplit[1]);
}

const Shared<Framebuffer>& RenderGraph::GetOutput() const
{
	return _backBuffer;
}

const RenderPass& RenderGraph::GetRenderPass(const String& name) const
{
	const auto result = std::find_if(_passes.begin(), _passes.end(), [&name](auto& pass)
	{
		return pass->GetName() == name;
	});

	Debug::Assert(result != _passes.end(), "Could not find Render pass: " + name);;
	return **result;
}

const RenderPass& RenderGraph::GetFinalRenderPass() const
{
	return *_passes.back();
}

void RenderGraph::LinkInputs(const RenderPass& renderPass)
{
	for (const auto& [name, input] : renderPass.GetInputs())
	{
		const auto& derivedPass = input->GetDerivedPass();

		Debug::Assert(!derivedPass.empty(),
		              "No derived pass set for output named " + input->GetName() + " (Render pass:" + renderPass.
		              GetName() + ")");

		if (derivedPass == "$")
		{
			const auto& matchName = input->GetDerivedPassOutput();
			const auto result = std::find_if(_globalOutputs.begin(), _globalOutputs.end(),
			                                 [&matchName](const Unique<Stream::Output>& elem)
			                                 {
				                                 return elem->GetName() == matchName;
			                                 });

			Debug::Assert(result != _globalOutputs.end(),
			              "Output named [" + input->GetDerivedPassOutput() + "] not found in globals");
			input->Bind(**result);
		}
		else
		{
			const auto result = std::find_if(_passes.begin(), _passes.end(),
			                                 [&derivedPass](const Unique<RenderPass>& elem)
			                                 {
				                                 return elem->GetName() == derivedPass;
			                                 });
			Debug::Assert(result != _passes.end(), "Render pass named [" + derivedPass + "] was not found");

			const auto& output = static_cast<const RenderPass&>(**result).GetOutput(input->GetDerivedPassOutput());
			input->Bind(output);
		}
	}
}

void RenderGraph::LinkGlobalInputs()
{
	for (auto& input : _globalInputs)
	{
		const auto& sourcePassName = input->GetDerivedPass();
		const auto result = std::find_if(_passes.begin(), _passes.end(),
		                                 [&sourcePassName](const Unique<RenderPass>& elem)
		                                 {
			                                 return elem->GetName() == sourcePassName;
		                                 });
		if (result != _passes.end())
		{
			auto& output = static_cast<const RenderPass&>(**result).GetOutput(input->GetDerivedPassOutput());
			input->Bind(output);
		}
	}
}
}
