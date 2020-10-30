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

	const Shared<EditorCamera> &GetEditorCamera() const { return m_EditorCamera; }
	const Shared<SceneRenderer::Target> &GetTarget() const override { return SceneRenderer::GetMainTarget(); }

private:
	Entity m_Entity;
	ViewportPane m_ViewportPane;
	Shared<EditorCamera> m_EditorCamera;

	Matrix4f m_SavedTransform;

};
}
