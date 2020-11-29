#pragma once

#include <Saffron.h>

namespace Se
{
class EditorLayer : public Layer, public Signaller {
public:
	struct Signals {
		static SignalAggregate<void> OnWantProjectSelector;
	};

public:
	enum class SceneState : int {
		Splash = 0, Edit = 1, Play = 2, Pause = 3
	};

	enum class SelectionMode {
		None = 0, Entity = 1, SubMesh = 2
	};

public:
	explicit EditorLayer(const std::shared_ptr<Project> &project);

	~EditorLayer() override = default;

	void OnAttach(std::shared_ptr<BatchLoader> &loader) override;
	void OnDetach() override;

	void OnUpdate() override;
	void OnGuiRender() override;
	void OnEvent(const Event &event) override;

	bool OnKeyboardPress(const KeyboardPressEvent &event);
	bool OnMouseButtonPressed(const MousePressEvent &event);
	bool OnWindowDropFiles(const WindowDropFilesEvent &event);

	void PromptNewScene();
	void PromptNewProject();
	void PromptImportScene();
	void PromptOpenProject();

	void SaveActiveProject() const;
	void SaveActiveScene() const;
	void LoadProjectScene(const Filepath &filepath);

private:
	Pair<Vector3f, Vector3f> CastRay(float mx, float my) const;
	Ray CastMouseRay() const;

	void OnGuiRenderMenuBar();
	void OnGuiRenderToolbar();

	void OnEntitySelected(Entity entity);
	void OnEntityUnselected(Entity entity);
	void OnEntityDeleted(Entity entity);
	void OnEntityCopied(Entity entity);

	void OnNewModelSpaceView(Entity entity);
	void OnSceneChange(std::shared_ptr<Scene> scene, Entity selection);
	void OnProjectChange(const std::shared_ptr<Project> &project);
	void OnWantProjectSelector();

	void OnPlay();
	void OnStop();

	float GetSnapValue() const;

	std::shared_ptr<ViewportPane> GetActiveViewportPane() const;
	std::shared_ptr<Scene> GetActiveScene() const;

private:
	int m_Style;
	int m_GizmoType = -1;

	std::shared_ptr<Project> m_Project;

	std::shared_ptr<EditorScene> m_EditorScene;
	std::shared_ptr<RuntimeScene> m_RuntimeScene;
	std::shared_ptr<ViewportPane> m_MainViewportPane;
	std::shared_ptr<ViewportPane> m_MiniViewportPane;
	ArrayList<Pair<std::shared_ptr<ModelSpaceScene>, std::shared_ptr<ViewportPane>>> m_ModelSpaceSceneViews;
	ArrayList<bool> m_DockedModelSpaceScene;
	mutable std::shared_ptr<Scene> m_LastFocusedScene;
	std::shared_ptr<Scene> m_CachedActiveScene;

	bool m_ReloadScriptOnPlay = true;

	EditorTerminal m_EditorTerminal;

	std::shared_ptr<Shader> m_BrushShader;
	std::shared_ptr<Material> m_SphereBaseMaterial;

	std::shared_ptr<Material> m_MeshMaterial;
	ArrayList<std::shared_ptr<MaterialInstance>> m_MetalSphereMaterialInstances, m_DielectricSphereMaterialInstances;

	// Editor resources
	Map<String, std::shared_ptr<Texture2D>> m_TexStore;

	std::shared_ptr<AssetPanel> m_AssetPanel;
	std::shared_ptr<EntityPanel> m_EntityPanel;
	std::shared_ptr<ScriptPanel> m_ScriptPanel;
	std::shared_ptr<ScenePanel> m_ScenePanel;
	Run::Handle m_AssetScriptRunHandle;

	SceneState m_SceneState;

	SelectionMode m_SelectionMode = SelectionMode::Entity;
	Entity m_SelectedEntity;
};
}
