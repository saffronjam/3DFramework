#pragma once

#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class EntityPanel : public ReferenceCounted
{
public:
	explicit EntityPanel(const Shared<Scene> &context);

	void OnGuiRender();

	void SetContext(const Shared<Scene> &context);
	void SetSelectedEntity(Entity entity);

private:
	void OnGuiRenderProperties();
	void OnGuiRenderMaterial();
	void OnGuiRenderMeshDebug();

	void DrawComponents(Entity entity);

private:
	Shared<Scene> m_Context;
	Entity m_SelectionContext;

	Map<String, Shared<Texture2D>> m_TexStore;
};
}