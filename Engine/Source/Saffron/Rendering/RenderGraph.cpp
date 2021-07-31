#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderGraph.h"

#include "Saffron/Rendering/RenderPasses/BufferClearPass.h"

namespace Se
{
RenderGraph::RenderGraph()
{
	{
		auto clearPass = std::make_unique<BufferClearPass>("clearBackBuffer");
		//clearPass->Connect()
	}

	{
		
	}
	

	Link();
}

void RenderGraph::Execute()
{
	for (auto& pass : _passes)
	{
		pass->Execute();
	}
}

void RenderGraph::Link()
{
}
}
