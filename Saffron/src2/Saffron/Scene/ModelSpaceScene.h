#pragma once

#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class ModelSpaceScene : public Scene
{
public:
	explicit ModelSpaceScene(Entity entity);

	void OnUpdate() override;
	void OnRender() override;
	void OnGuiRender() override;

	void EnableCamera() { m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->Enable(); }
	void DisableCamera() { m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->Disable(); }

	Entity GetModelEntity() const { return m_Entity; }
	const Shared<EditorCamera> &GetEditorCamera() const { return m_EditorCamera; }
	const Shared<SceneRenderer::Target> &GetTarget() const override { return m_Target; }

	void SetViewportSize(Uint32 width, Uint32 height) override;
	void SetSelectedEntity(Entity entity) override;

private:
	Entity m_Entity;
	Shared<SceneRenderer::Target> m_Target;
	Shared<EditorCamera> m_EditorCamera;

	Matrix4f m_SavedTransform;

};
}
