#pragma once

#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class EntityPanel : public MemManaged<EntityPanel>
{
public:
	explicit EntityPanel(const std::shared_ptr<Scene> &context);

	void OnGuiRender();

	void SetContext(const std::shared_ptr<Scene> &context);
	void SetSelectedEntity(Entity entity);

private:
	void OnGuiRenderProperties();
	void OnGuiRenderMaterial();
	void OnGuiRenderMeshDebug();

	void DrawComponents(Entity entity);

private:
	std::shared_ptr<Scene> m_Context;
	Entity m_SelectionContext;

	Map<String, std::shared_ptr<Texture2D>> m_TexStore;
};
}