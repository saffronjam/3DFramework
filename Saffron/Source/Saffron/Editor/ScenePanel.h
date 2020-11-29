#pragma once

#include "Saffron/Base.h"
#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class ScenePanel : public MemManaged<ScenePanel>, public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<Entity> OnEntityDeleted;
		static SignalAggregate<Entity> OnEntityCopied;
		static SignalAggregate<Entity> OnNewSelection;
		static SignalAggregate<Entity> OnViewInModelSpace;
	};

public:
	explicit ScenePanel(const std::shared_ptr<Scene> &context);

	void OnGuiRender(const std::shared_ptr<ScriptPanel> &scriptPanel);

	void SetContext(const std::shared_ptr<Scene> &scene) { m_Context = scene; }
	void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

private:
	void OnCreateEntity(bool viewModal, const std::shared_ptr<ScriptPanel> &scriptPanel);
	void OnCreateMesh();
	void OnCreateDirectionalLight();
	void OnCreateSkylight();

	void DrawEntityNode(Entity entity);
	void DrawMeshNode(const std::shared_ptr<Mesh> &mesh, UUID &entityUUID) const;
	void MeshNodeHierarchy(const std::shared_ptr<Mesh> &mesh, aiNode *node, const Matrix4f &parentTransform = Matrix4f(1.0f), Uint32 level = 0) const;

private:
	std::shared_ptr<Scene> m_Context;
	Entity m_SelectionContext;
};
}
