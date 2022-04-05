#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderPasses/ShadowMapPass.h"

#include "Saffron/Graphics/Model.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"

#undef min
#undef max

namespace Se
{
ShadowMapPass::ShadowMapPass(const std::string& name, struct SceneCommon& sceneCommon) :
	RenderPass(name, sceneCommon)
{
	//_shadowMap->SetDepthClearValue(0.0f);

	RegisterOutput("Target0", _shadowMap);
}

void ShadowMapPass::OnSetupFinished()
{
}

void ShadowMapPass::Execute()
{
	_shadowMap->Bind();
	_shadowMap->Clear();

	Renderer::ResetRenderState();

	_shadowMap->Unbind();
}
}
