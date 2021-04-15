#pragma once

#include "Saffron/Base.h"
#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class ScenePanel : public MemManaged<ScenePanel>
{
public:
	explicit ScenePanel(const Shared<Scene>& context);

	void OnGuiRender(const Shared<ScriptPanel>& scriptPanel);

	void SetContext(const Shared<Scene>& scene) { m_Context = scene; }

	void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

private:
	void OnCreateEntity(bool viewModal, const Shared<ScriptPanel>& scriptPanel);
	void OnCreateMesh();
	void OnCreateDirectionalLight();
	void OnCreateSkylight();

	void DrawEntityNode(Entity entity);
	void DrawMeshNode(const Shared<Mesh>& mesh, UUID& entityUUID) const;
	void MeshNodeHierarchy(const Shared<Mesh>& mesh, aiNode* node, const Matrix4f& parentTransform = Matrix4f(1.0f),
	                       Uint32 level = 0) const;

public:
	mutable EventSubscriberList<Entity> EntityDeleted;
	mutable EventSubscriberList<Entity> EntityCopied;
	mutable EventSubscriberList<Entity> NewSelection;
	mutable EventSubscriberList<Entity> ViewInModelSpace;

private:
	Shared<Scene> m_Context;
	Entity m_SelectionContext;
};
}
