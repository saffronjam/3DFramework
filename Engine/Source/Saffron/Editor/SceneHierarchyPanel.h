#pragma once

#include "Saffron/Base.h"
#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class SceneHierarchyPanel : public Managed
{
public:
	explicit SceneHierarchyPanel(const Shared<Scene>& context);

	void OnGuiRender(const Shared<ScriptPanel>& scriptPanel);

	void SetContext(const Shared<Scene>& scene) { _context = scene; }

	void SetSelectedEntity(Entity entity) { _selectionContext = entity; }

private:
	void OnCreateEntity(bool viewModal, const Shared<ScriptPanel>& scriptPanel);
	void OnCreateMesh();
	void OnCreateDirectionalLight();
	void OnCreateSkylight();

	void DrawEntityNode(Entity entity);
	void DrawMeshNode(const Shared<Mesh>& mesh, UUID& entityUUID) const;
	void MeshNodeHierarchy(const Shared<Mesh>& mesh, aiNode* node, const Matrix4& parentTransform = Matrix4(1.0f),
	                       uint level = 0) const;

public:
	mutable EventSubscriberList<Entity> EntityDeleted;
	mutable EventSubscriberList<Entity> EntityCopied;
	mutable EventSubscriberList<Entity> NewSelection;
	mutable EventSubscriberList<Entity> ViewInModelSpace;

private:
	Shared<Scene> _context;
	Entity _selectionContext;
};
}
