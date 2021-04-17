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
	int _style = static_cast<int>(Gui::Style::Dark);
	int _gizmoType = -1;

	Shared<Project> _project;

	Shared<EditorScene> _editorScene;
	Shared<RuntimeScene> _runtimeScene;
	Shared<ViewportPane> _mainViewportPane;
	ArrayList<bool> _dockedModelSpaceScene;
	mutable Shared<Scene> _lastFocusedScene;
	Shared<Scene> _cachedActiveScene;

	bool _reloadScriptOnPlay = true;

	EditorTerminal _editorTerminal;

	Shared<Shader> _brushShader;
	Shared<Material> _sphereBaseMaterial;

	Shared<Material> _meshMaterial;
	ArrayList<Shared<MaterialInstance>> _metalSphereMaterialInstances, _dielectricSphereMaterialInstances;

	// Editor resources
	Map<String, Shared<Texture2D>> _texStore;

	Shared<AssetPanel> _assetPanel;
	Shared<EntityComponentsPanel> _entityPanel;
	Shared<ScriptPanel> _scriptPanel;
	Shared<SceneHierarchyPanel> _scenePanel;
	Run::Handle _assetScriptRunHandle;

	SceneState _sceneState;

	SelectionMode _selectionMode = SelectionMode::Entity;
	Entity _selectedEntity;
};
}
