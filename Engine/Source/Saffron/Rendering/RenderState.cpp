#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderState.h"

namespace Se
{
const RenderState RenderState::Default = DepthTest_Less | Rasterizer_CullFront | Rasterizer_Fill | Sampler_Anisotropic;

RenderState::RenderState(long state) :
	State(state)
{
}
}
