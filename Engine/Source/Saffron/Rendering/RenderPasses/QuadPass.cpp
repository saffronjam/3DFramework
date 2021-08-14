#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/QuadPass.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
QuadPass::QuadPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon)
{
}

void QuadPass::Execute()
{
}
}
