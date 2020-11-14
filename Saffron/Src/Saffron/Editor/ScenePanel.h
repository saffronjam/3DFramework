#pragma once

#include "Saffron/Base.h"
#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class ScenePanel : public ReferenceCounted, public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<Entity> OnDelete;
		static SignalAggregate<Entity> OnNewSelection;
		static SignalAggregate<Entity> OnViewInModelSpace;
	};

public:
	explicit ScenePanel(const Shared<Scene> &context);

	void OnGuiRender(const Shared<ScriptPanel> &scriptPanel);

	void SetContext(const Shared<Scene> &scene) { m_Context = scene; }
	void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

private:
	void DrawEntityNode(Entity entity);
	void DrawMeshNode(const Shared<Mesh> &mesh, UUID &entityUUID) const;
	void MeshNodeHierarchy(const Shared<Mesh> &mesh, aiNode *node, const Matrix4f &parentTransform = Matrix4f(1.0f), Uint32 level = 0) const;

private:
	Shared<Scene> m_Context;
	Entity m_SelectionContext;
};
}
