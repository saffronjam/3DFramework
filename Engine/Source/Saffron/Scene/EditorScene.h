#pragma once

#include "Saffron/Scene/Scene.h"

namespace Se
{
class EditorScene : public Scene
{
public:
	explicit EditorScene(Filepath filepath);

	void OnUpdate() override;
	void OnRender() override;
	void OnGuiRender() override;

	void EnableCamera() { m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->Enable(); }

	void DisableCamera() { m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->Disable(); }

	const Shared<EditorCamera>& GetEditorCamera() const { return m_EditorCamera; }

	const Filepath& GetFilepath() const { return m_Filepath; }

	void SetSelectedEntity(Entity entity) override;
	void SetViewportSize(Uint32 width, Uint32 height) override;

	void Save() const;

private:
	Shared<EditorCamera> m_EditorCamera;
	Filepath m_Filepath;
};
}
