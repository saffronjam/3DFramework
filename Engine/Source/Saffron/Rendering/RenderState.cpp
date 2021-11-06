#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderState.h"

namespace Se
{
const RenderState RenderState::Default = DepthTest_Less | DepthWriteMask_All | Rasterizer_CullFront | Rasterizer_Fill |
	Topology_TriangleList;

RenderState::RenderState(long state) :
	State(state)
{
}
}
