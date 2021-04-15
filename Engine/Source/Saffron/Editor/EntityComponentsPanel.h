#pragma once

#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class EntityComponentsPanel
{
public:
	explicit EntityComponentsPanel(const Shared<Scene>& context);

	void OnGuiRender();

	void SetContext(const Shared<Scene>& context);
	void SetSelectedEntity(Entity entity);

private:
	void OnGuiRenderProperties();
	void OnGuiRenderMaterial();
	void OnGuiRenderMeshDebug();

	void DrawComponents(Entity entity);
	static bool DrawVec3Control(const String& label, Vector3f& value, float resetValue = 0.0f, float columnWidth = 100.0f);

private:
	Shared<Scene> m_Context;
	Entity m_SelectionContext;

	Map<String, Shared<Texture2D>> m_TexStore;
};
}
