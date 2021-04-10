#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderChannel.h"
#include "Saffron/Rendering/Resources.h"

namespace Se
{
class Scene;
class Mesh;

class SceneRenderer
{
public:
	SceneRenderer();
	SceneRenderer(const SceneRenderer&) = delete;
	SceneRenderer& operator=(const SceneRenderer&) = delete;
	~SceneRenderer();

	void Begin(const Shared<Scene>& scene);
	void End();

	void Submit(const Shared<Mesh>& mesh, RenderChannel renderChannel);

	Scene& GetActiveScene();
	const Scene& GetActiveScene() const;

	static SceneRenderer& Instance()
	{
		SE_CORE_ASSERT(_instance != nullptr && "Renderer was not instansiated")
		return *_instance;
	}

private:
	static SceneRenderer* _instance;

	UnorderedMap<RenderChannel, Shared<Mesh>> _submitions;

	Shared<Program> _commonProgram;

	Shared<Scene> _activeScene = nullptr;
};
}
