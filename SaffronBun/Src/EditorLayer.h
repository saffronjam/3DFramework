#pragma once

#include "Saffron.h"

#include <string>

#include "Saffron/Base.h"
#include "Saffron/Editor/EditorCamera.h"
#include "Saffron/Editor/EditorTerminal.h"
#include "Saffron/Editor/EntityPanel.h"
#include "Saffron/Gui/GuiLayer.h"

namespace Se
{
class EditorLayer : public Layer
{
public:

	struct AlbedoInput
	{
		Vector3f Color = { 0.972f, 0.96f, 0.915f }; // Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
		Shared<Texture2D> TextureMap;
		bool sRGB = true;
		bool UseTexture = false;
	};

	struct NormalInput
	{
		Shared<Texture2D> TextureMap;
		bool UseTexture = false;
	};

	struct MetalnessInput
	{
		float Value = 1.0f;
		Shared<Texture2D> TextureMap;
		bool UseTexture = false;
	};

	struct RoughnessInput
	{
		float Value = 0.2f;
		Shared<Texture2D> TextureMap;
		bool UseTexture = false;
	};

	enum class SceneState : int
	{
		Edit = 0, Play = 1, Pause = 2
	};

	enum class SelectionMode
	{
		None = 0, Entity = 1, SubMesh = 2
	};

public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;

	void OnGuiRender() override;
	void OnEvent(const Event &event) override;
	bool OnKeyboardPressEvent(const KeyboardPressEvent &event);
	bool OnMouseButtonPressed(const MousePressEvent &event);
	bool OnWindowDropFiles(const WindowDropFilesEvent &event);

	void NewScenePrompt();
	void OpenScenePrompt();
	void SaveSceneAsPrompt();

	void SaveActiveScene() const;
	void LoadNewScene(const String &filepath);
private:
	std::pair<Vector3f, Vector3f> CastRay(float mx, float my) const;

	void OnGuiRenderMenuBar();
	void OnGuiRenderToolbar();
	void OnSelected(Entity entity);
	void OnUnselected(Entity entity);
	void OnEntityDeleted(Entity entity);
	void OnNewModelSpaceView(Entity entity);
	Ray CastMouseRay() const;

	void OnSceneChange();
	void OnScenePlay();
	void OnSceneStop();

	void UpdateWindowTitle(const String &sceneName);

	float GetSnapValue() const;
private:
	int m_Style;
	int m_GizmoType = -1;
	bool m_ModelSpaceEditor = false;

	Shared<Scene> m_RuntimeScene, m_EditorScene;
	Map<Entity, Pair<Shared<Scene>, ViewportPane>> m_EntityModelSpacesMap;
	Shared<SceneRenderer::Target> m_MainTarget, m_MiniTarget;

	Filepath m_SceneFilePath;
	bool m_ReloadScriptOnPlay = true;

	ViewportPane m_MainViewport, m_MiniViewport;
	EditorCamera m_EditorCamera;
	EditorTerminal m_EditorTerminal;

	Shared<Shader> m_BrushShader;
	Shared<Material> m_SphereBaseMaterial;

	Shared<Material> m_MeshMaterial;
	ArrayList<Shared<MaterialInstance>> m_MetalSphereMaterialInstances, m_DielectricSphereMaterialInstances;

	// Editor resources
	Map<String, Shared<Texture2D>> m_TexStore;

	Shared<AssetPanel> m_AssetPanel;
	Shared<EntityPanel> m_EntityPanel;
	Shared<ScriptPanel> m_ScriptPanel;

	SceneState m_SceneState = SceneState::Edit;

	SelectionMode m_SelectionMode = SelectionMode::Entity;
	Entity m_SelectedEntity;
};

}
