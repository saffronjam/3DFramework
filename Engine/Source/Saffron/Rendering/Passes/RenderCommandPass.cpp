#include "SaffronPCH.h"

#include "Saffron/Rendering/Passes/RenderCommandPass.h"

namespace Se
{
void RenderCommandPass::Execute()
{
	for(auto& command : _commands)
	{
	}
}

void RenderCommandPass::Add(const RenderCommand& command)
{
	_commands.push_back(command);
}
}
