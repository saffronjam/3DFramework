#pragma once

#include "Saffron/Base.h"

#include "Saffron/Rendering/DrawCommand.h"
#include "Saffron/Rendering/RenderChannel.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
class RenderPass;
class Scene;

class RenderGraph
{
public:
	void Execute(const SceneRenderer::JobContainer &jobs);

	const ArrayList<DrawCommand> &GetDrawCommands(RenderChannel channel) const;
	const Scene &GetActiveScene() const;

protected:
	void Add(Unique<RenderPass> renderPass);

private:
	ArrayList<Unique<RenderPass>> _renderPasses;
	const SceneRenderer::JobContainer *_jobs = nullptr;
	const Scene *_activeScene = nullptr;
};
}
