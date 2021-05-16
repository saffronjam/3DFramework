#pragma once

#include "Saffron/Scene/Scene.h"

namespace Se
{
class EditorScene : public Scene
{
public:
	explicit EditorScene(Path filepath);

	void OnUpdate() override;
	void OnRender() override;
	void OnGuiRender() override;

	void EnableCamera() { _sceneEntity.GetComponent<EditorCameraComponent>().Camera->Enable(); }

	void DisableCamera() { _sceneEntity.GetComponent<EditorCameraComponent>().Camera->Disable(); }

	const Shared<EditorCamera>& GetEditorCamera() const { return _editorCamera; }

	const Path& GetFilepath() const { return _filepath; }

	void SetSelectedEntity(Entity entity) override;
	void SetViewportSize(uint width, uint height) override;

	void Save() const;

private:
	Shared<EditorCamera> _editorCamera;
	Path _filepath;
};
}
