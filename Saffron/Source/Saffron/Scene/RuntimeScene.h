#pragma once

#include "Saffron/Scene/Scene.h"

namespace Se
{
class RuntimeScene : public Scene
{
public:
	explicit RuntimeScene(std::shared_ptr<Scene> copyFrom);

	void OnUpdate() override;
	void OnRender() override;
	void OnGuiRender() override;
	void OnStart();
	void OnStop();

	void Escape();
	void Return();

	const std::shared_ptr<EditorCamera> &GetDebugCamera() const { return m_DebugCamera; }
	const std::shared_ptr<SceneRenderer::Target> &GetTarget() const override { return SceneRenderer::GetMainTarget(); }

private:
	std::shared_ptr<EditorCamera> m_DebugCamera;
	bool m_DebugCameraActivated = false;
};
}

