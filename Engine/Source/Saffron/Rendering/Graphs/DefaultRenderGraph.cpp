#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Graphs/DefaultRenderGraph.h"
#include "Saffron/Rendering/Passes/GeometryPass.h"

namespace Se
{
DefaultRenderGraph::DefaultRenderGraph()
{
	Add(CreateUnique<GeometryPass>());
}
}
