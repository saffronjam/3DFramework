#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderGraph.h"

namespace Se
{
void RenderGraph::Execute()
{
	for (auto& pass : _renderPasses)
	{
		pass->Execute();
	}
}

void RenderGraph::Add(Unique<RenderPass> renderPass)
{
	_renderPasses.push_back(Move(renderPass));
}
}
