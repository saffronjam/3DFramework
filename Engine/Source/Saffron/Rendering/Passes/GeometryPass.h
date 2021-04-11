#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class GeometryPass : public RenderPass
{
public:
	GeometryPass();
	
	void Execute(const RenderGraph& rendergraph) override;

private:
	Shared<Program> _geoProgram;
};
}
