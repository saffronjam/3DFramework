#pragma once

#include <Saffron.h>

namespace Se
{
class EditorLayer : public Layer
{
public:
	enum class SceneState : int
	{
		Splash = 0,
		Edit = 1,
		Play = 2,
		Pause = 3
	};

	enum class SelectionMode
	{
		None = 0,
		Entity = 1,
		SubMesh = 2
	};

public:
	explicit EditorLayer(const Shared<Project>& project);

	~EditorLayer() override = default;

	void OnAttach(Shared<BatchLoader>& loader) override;
	void OnDetach() override;

	void OnUpdate() override;
	void OnGuiRender() override;

	bool OnKeyboardPress(const KeyPressedEvent& event);
	bool OnMouseButtonPressed(const MouseButtonPressedEvent& event);
	bool OnWindowDropFiles(const WindowDroppedFilesEvent& event);

	void PromptNewScene();
	void PromptNewProject();
	void PromptImportScene();
	void PromptOpenProject();

	void SaveActiveProject() const;
	void SaveActiveScene() const;
	void LoadProjectScene(const Filepath& filepath);

private:
	Pair<Vector3f, Vector3f> CastRay(float mx, float my) const;
	Ray CastMouseRay() const;

	void OnGuiRenderMenuBar();
	void OnGuiRenderToolbar();

	bool OnEntitySelected(Entity entity);
	bool OnEntityUnselected(Entity entity);
	bool OnEntityDeleted(Entity entity);
	bool OnEntityCopied(Entity entity);

	bool OnNewModelSpaceView(Entity entity);
	void OnSceneChange(Shared<Scene> scene, Entity selection);
	void OnProjectChange(const Shared<Project>& project);
	void OnWantProjectSelector();

	void OnPlay();
	void OnStop();

	float GetSnapValue() const;

	Shared<ViewportPane> GetActiveViewportPane() const;
	Shared<Scene> GetActiveScene() const;

public:
	EventSubscriberList<void> WantProjectSelector;

private:
	int m_Style = static_cast<int>(Gui::Style::Dark);
	int m_GizmoType = -1;

	Shared<Project> m_Project;

	Shared<EditorScene> m_EditorScene;
	Shared<RuntimeScene> m_RuntimeScene;
	Shared<ViewportPane> m_MainViewportPane;
	ArrayList<bool> m_DockedModelSpaceScene;
	mutable Shared<Scene> m_LastFocusedScene;
	Shared<Scene> m_CachedActiveScene;

	bool m_ReloadScriptOnPlay = true;

	EditorTerminal m_EditorTerminal;

	Shared<Shader> m_BrushShader;
	Shared<Material> m_SphereBaseMaterial;

	Shared<Material> m_MeshMaterial;
	ArrayList<Shared<MaterialInstance>> m_MetalSphereMaterialInstances, m_DielectricSphereMaterialInstances;

	// Editor resources
	Map<String, Shared<Texture2D>> m_TexStore;

	Shared<AssetPanel> m_AssetPanel;
	Shared<EntityComponentsPanel> m_EntityPanel;
	Shared<ScriptPanel> m_ScriptPanel;
	Shared<SceneHierarchyPanel> m_ScenePanel;
	Run::Handle m_AssetScriptRunHandle;

	SceneState m_SceneState;

	SelectionMode m_SelectionMode = SelectionMode::Entity;
	Entity m_SelectedEntity;
};
}
