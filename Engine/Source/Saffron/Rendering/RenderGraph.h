#pragma once

#include <memory>

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class RenderGraph
{
public:
	RenderGraph();

	void Execute();

private:
	void Link();

private:
	std::vector<std::unique_ptr<RenderPass>> _passes;
};
}
