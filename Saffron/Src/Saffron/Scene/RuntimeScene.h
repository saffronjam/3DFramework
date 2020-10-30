#pragma once

#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class RuntimeScene : public Scene
{
public:
	explicit RuntimeScene(String name = "Runtime Scene", Shared<Scene> copyFrom);

	void OnUpdate() override;
	void OnRender() override;
	void OnGuiRender() override;
	void OnStart();
	void OnStop();

	const Shared<SceneRenderer::Target> &GetTarget() const override { return SceneRenderer::GetMainTarget(); }

private:
	ViewportPane m_ViewportPane;
};
}

