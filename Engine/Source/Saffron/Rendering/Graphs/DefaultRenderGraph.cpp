#include "SaffronPCH.h"

#include "Saffron/Rendering/Graphs/DefaultRenderGraph.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/Renderer.h"

// Passes
#include "Saffron/Rendering/Passes/BloomBlurPass.h"
#include "Saffron/Rendering/Passes/CompositePass.h"
#include "Saffron/Rendering/Passes/ShadowMapPass.h"
#include "Saffron/Rendering/Passes/GeometryPass.h"

namespace Se
{
DefaultRenderGraph::DefaultRenderGraph()
{	
	{
		auto pass = CreateUnique<ShadowMapPass>("shadowMap");
		AddPass(Move(pass));
	}

	{
		auto pass = CreateUnique<GeometryPass>("geometry");
		pass->SetStreamLinkage("shadowMap.shadowMap0", "shadowMap0");
		pass->SetStreamLinkage("shadowMap.shadowMap1", "shadowMap1");
		pass->SetStreamLinkage("shadowMap.shadowMap2", "shadowMap2");
		pass->SetStreamLinkage("shadowMap.shadowMap3", "shadowMap3");
		AddPass(Move(pass));
	}

	{
		auto pass = CreateUnique<CompositePass>("composite");
		pass->SetStreamLinkage("geometry.buffer", "buffer");
		AddPass(Move(pass));
	}

	SetGlobalStreamLinkage("composite.buffer", "backBuffer");
	RenderGraph::LinkGlobalInputs();
}

void DefaultRenderGraph::Execute()
{
	for (auto& pass : _passes)
	{
		pass->Execute();
	}
}
}
