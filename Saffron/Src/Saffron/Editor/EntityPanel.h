#pragma once

#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class EntityPanel : public RefCounted
{
public:
	explicit EntityPanel(const Ref<Scene> &context);

	void OnGuiRender(const Ref<ScriptPanel> &scriptPanel);

	void SetContext(const Ref<Scene> &context);
	void SetSelected(Entity entity);
	void SetSelectionChangedCallback(const std::function<void(Entity)> &func) { m_SelectionChangedCallback = func; }
	void SetEntityDeletedCallback(const std::function<void(Entity)> &func) { m_EntityDeletedCallback = func; }

private:
	void OnGuiRenderSceneHierarchy(const Ref<ScriptPanel> &scriptPanel);
	void OnGuiRenderMaterial();
	void OnGuiRenderMeshDebug();

	void DrawEntityNode(Entity entity);
	void DrawMeshNode(const Ref<Mesh> &mesh, UUID &entityUUID) const;
	void MeshNodeHierarchy(const Ref<Mesh> &mesh, aiNode *node, const glm::mat4 &parentTransform = glm::mat4(1.0f), Uint32 level = 0) const;
	void DrawComponents(Entity entity);

private:
	Ref<Scene> m_Context;
	Entity m_SelectionContext;

	std::map<std::string, Ref<Texture2D>> m_TexStore;

	std::function<void(Entity)> m_SelectionChangedCallback, m_EntityDeletedCallback;
};
}