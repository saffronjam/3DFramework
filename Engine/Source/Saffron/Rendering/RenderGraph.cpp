#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
void RenderGraph::Execute(const SceneRenderer::JobContainer& jobs)
{
	_activeScene = &SceneRenderer::Instance().GetActiveScene();
	_jobs = &jobs;
	for (auto& pass : _renderPasses)
	{
		pass->Execute(*this);
	}
	_jobs = nullptr;
	_activeScene = nullptr;
}

const ArrayList<DrawCommand>& RenderGraph::GetDrawCommands(RenderChannel channel) const
{
	SE_CORE_ASSERT(_jobs != nullptr, "Render graph was not in a running state");
	return _jobs->at(channel);
}

const Scene& RenderGraph::GetActiveScene() const
{
	SE_CORE_ASSERT(_activeScene != nullptr, "Render graph was not in a running state");
	return *_activeScene;
}

void RenderGraph::Add(Unique<RenderPass> renderPass)
{
	_renderPasses.push_back(Move(renderPass));
}
}
