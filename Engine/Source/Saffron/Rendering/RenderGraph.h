#pragma once

#include "Saffron/Base.h"

#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class RenderGraph
{
public:
	void Execute();

	void Add(Unique<RenderPass> renderPass);

private:
	ArrayList<Unique<RenderPass>> _renderPasses;
};
}
