#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/DrawCommand.h"
#include "Saffron/Rendering/RenderChannel.h"

namespace Se
{
class Scene;
class RenderGraph;

class SceneRenderer
{
public:
	using JobContainer = UnorderedMap<RenderChannel, ArrayList<DrawCommand>>;

public:
	SceneRenderer();
	SceneRenderer(const SceneRenderer&) = delete;
	SceneRenderer& operator=(const SceneRenderer&) = delete;
	~SceneRenderer() = default;

	void Begin(const Shared<Scene>& scene);
	void End();

	void Submit(const Shared<Mesh>& mesh, const Shared<Material>& material, RenderChannel renderChannel);

	Scene& GetActiveScene();
	const Scene& GetActiveScene() const;

	static SceneRenderer& Instance()
	{
		SE_CORE_ASSERT(_instance != nullptr && "Renderer was not instansiated")
		return *_instance;
	}

private:
	void ClearJobs();

private:
	static SceneRenderer* _instance;

	Unique<RenderGraph> _renderGraph;
	JobContainer _jobs;

	Shared<Scene> _activeScene = nullptr;
};
}
