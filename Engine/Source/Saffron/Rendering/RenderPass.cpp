#include "SaffronPCH.h"

#include "Saffron/Core/GenUtils.h"
#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
RenderPass::RenderPass(String name) :
	_name(Move(name))
{
}

bool RenderPass::operator==(const RenderPass& rhs) const
{
	return _uuid == rhs._uuid;
}

const String& RenderPass::GetName() const
{
	return _name;
}

void RenderPass::SetStreamLinkage(const String& outputName, const String& inputName)
{
	auto& input = GetInput(inputName);
	const auto inputSplit = GenUtils::SplitString(outputName, ".");
	Debug::Assert(inputSplit.size() == 2,
	               "Input linkage target has incorrect format. Expected format: \"<pass>.<input>\". Got: " + inputName);
	input.SetDerived(Move(inputSplit[0]), Move(inputSplit[1]));
}

const HashMap<String, Unique<Stream::Input>>& RenderPass::GetInputs() const
{
	return _inputs;
}

const HashMap<String, Unique<Stream::Output>>& RenderPass::GetOutputs() const
{
	return _outputs;
}

const Stream::Input& RenderPass::GetInput(const String& name) const
{
	return const_cast<RenderPass*>(this)->GetInput(name);
}

const Stream::Output& RenderPass::GetOutput(const String& name) const
{
	return const_cast<RenderPass*>(this)->GetOutput(name);
}

void RenderPass::AddInput(Unique<Stream::Input> input)
{
	_inputs.insert(CreatePair(input->GetName(), Move(input)));
}

void RenderPass::AddOutput(Unique<Stream::Output> output)
{
	_outputs.insert(CreatePair(output->GetName(), Move(output)));
}

Stream::Input& RenderPass::GetInput(const String& name)
{
	return *_inputs.at(name);
}

Stream::Output& RenderPass::GetOutput(const String& name)
{
	return *_outputs.at(name);
}
}
