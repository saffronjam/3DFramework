#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class DefaultRenderGraph : public RenderGraph
{
public:
	DefaultRenderGraph();
	
	void Execute() override;
};
}
