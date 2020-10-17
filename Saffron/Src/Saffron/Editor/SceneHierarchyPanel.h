#pragma once

#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/Entity.h"

namespace Se
{
class SceneHierarchyPanel
{
public:
	explicit SceneHierarchyPanel(const Ref<Scene> &scene);

	void SetContext(const Ref<Scene> &context);
	void SetSelected(Entity entity);
	void SetSelectionChangedCallback(const std::function<void(Entity)> &func) { m_SelectionChangedCallback = func; }
	void SetEntityDeletedCallback(const std::function<void(Entity)> &func) { m_EntityDeletedCallback = func; }

	void OnImGuiRender();
private:
	void DrawEntityNode(Entity entity);
	void DrawMeshNode(const Ref<Mesh> &mesh, UUID &entityUUID) const;
	void MeshNodeHierarchy(const Ref<Mesh> &mesh, aiNode *node, const glm::mat4 &parentTransform = glm::mat4(1.0f), Uint32 level = 0) const;
	void DrawComponents(Entity entity);
private:
	Ref<Scene> m_Context;
	Entity m_SelectionContext;

	std::string m_CachedNewModuleName = "NONE";

	std::function<void(Entity)> m_SelectionChangedCallback, m_EntityDeletedCallback;
};
}