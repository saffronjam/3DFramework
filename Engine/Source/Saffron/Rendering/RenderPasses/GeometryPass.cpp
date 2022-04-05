#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/GeometryPass.h"

#include "Saffron/Graphics/Model.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/Binders/Binder.h"

namespace Se
{
GeometryPass::GeometryPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon)
{
	RegisterInput("ShadowMap0", _shadowMap);
	RegisterInput("Target", _target);
	RegisterOutput("Target", _target);
}

void GeometryPass::OnUi()
{
}

void GeometryPass::Execute()
{
	auto& common = SceneCommon();

	// Setup renderer
	Renderer::ResetRenderState();
	Renderer::SetViewportSize(_target->Width(), _target->Height());
}


void GeometryPass::SetViewportSize(uint width, uint height)
{
	_target->Resize(width, height);
}
}
