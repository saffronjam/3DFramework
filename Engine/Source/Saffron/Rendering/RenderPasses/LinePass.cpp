#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/LinePass.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
LinePass::LinePass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon)
{
	RegisterInput("Target", _target);
	RegisterOutput("Target", _target);
}

void LinePass::Execute()
{
	const auto& common = SceneCommon();

	// If no lines are submitted, skip pass
	if (common.LinesVertices.empty()) return;

	// Setup renderer
	Renderer::SetRenderState(_renderState);
	Renderer::SetViewportSize(_target->Width(), _target->Height());

}
}
