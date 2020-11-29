#pragma once

#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Scene/Scene.h"

namespace  Se
{
class EditorScene : public Scene
{
public:
	EditorScene(Filepath filepath);

	void OnUpdate() override;
	void OnRender() override;
	void OnGuiRender() override;
	void OnEvent(const Event &event) override;

	void EnableCamera() { m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->Enable(); }
	void DisableCamera() { m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->Disable(); }

	const std::shared_ptr<EditorCamera> &GetEditorCamera() const { return m_EditorCamera; }
	const std::shared_ptr<SceneRenderer::Target> &GetTarget() const override { return SceneRenderer::GetMainTarget(); }
	const std::shared_ptr<SceneRenderer::Target> &GetMiniTarget() const { return m_MiniTarget; };
	const Filepath &GetFilepath() const { return m_Filepath; }

	void SetSelectedEntity(Entity entity) override;
	void SetViewportSize(Uint32 width, Uint32 height) override;

	void Save() const;

private:
	std::shared_ptr<SceneRenderer::Target> m_MiniTarget;
	std::shared_ptr<EditorCamera> m_EditorCamera;
	Filepath m_Filepath;
};
}
