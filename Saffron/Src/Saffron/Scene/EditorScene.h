#pragma once

#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Scene/Scene.h"

namespace  Se
{
class EditorScene : public Scene
{
public:
	EditorScene(String name = "Editor Scene");

	void OnUpdate() override;
	void OnRender() override;
	void OnGuiRender() override;
	void OnEvent(const Event &event) override;

	void EnableCamera() { m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->Enable(); }
	void DisableCamera() { m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->Disable(); }

	const Shared<EditorCamera> &GetEditorCamera() const { return m_EditorCamera; }
	const Shared<SceneRenderer::Target> &GetTarget() const override { return SceneRenderer::GetMainTarget(); }

	void SetSelectedEntity(Entity entity) override;

private:
	Shared<SceneRenderer::Target> m_MiniTarget;
	Shared<EditorCamera> m_EditorCamera;
};
}
