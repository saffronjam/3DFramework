#pragma once

#include "Saffron.h"

#include <string>

#include "Saffron/Editor/EditorCamera.h"
#include "Saffron/Editor/SceneHierarchyPanel.h"
#include "Saffron/Gui/GuiLayer.h"

namespace Se
{

class EditorLayer : public Layer
{
public:
	struct SelectedSubmesh
	{
		Entity Entity;
		Submesh *Mesh = nullptr;
		float Distance = 0.0f;
	};

	struct AlbedoInput
	{
		glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; // Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
		Ref<Texture2D> TextureMap;
		bool sRGB = true;
		bool UseTexture = false;
	};

	struct NormalInput
	{
		Ref<Texture2D> TextureMap;
		bool UseTexture = false;
	};

	struct MetalnessInput
	{
		float Value = 1.0f;
		Ref<Texture2D> TextureMap;
		bool UseTexture = false;
	};

	struct RoughnessInput
	{
		float Value = 0.2f;
		Ref<Texture2D> TextureMap;
		bool UseTexture = false;
	};

public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;

	void OnImGuiRender() override;
	void OnEvent(const Event &event) override;
	bool OnKeyboardPressEvent(const KeyboardPressEvent &event);
	bool OnMouseButtonPressed(const MousePressEvent &event);
	bool OnWindowDropFiles(const WindowDropFilesEvent &event);

	void ShowBoundingBoxes(bool show, bool onTop = false);
	void SelectEntity(Entity entity);

	void NewScenePrompt();
	void OpenScenePrompt();
	void SaveSceneAsPrompt();

	void SaveActiveScene() const;
	void LoadNewScene(const std::string &filepath);
private:
	std::pair<glm::vec3, glm::vec3> CastRay(float mx, float my) const;

	void OnSelected(const SelectedSubmesh &selectionContext);
	void OnEntityDeleted(Entity e);
	Ray CastMouseRay() const;

	void OnSceneChange();
	void OnScenePlay();
	void OnSceneStop();

	void UpdateWindowTitle(const std::string &sceneName);

	float GetSnapValue() const;
private:
	int m_Style;

	Scope<SceneHierarchyPanel> m_SceneHierarchyPanel;

	Ref<Scene> m_RuntimeScene, m_EditorScene;
	std::filesystem::path m_SceneFilePath;
	bool m_ReloadScriptOnPlay = true;

	EditorCamera m_EditorCamera;

	Ref<Shader> m_BrushShader;
	Ref<Material> m_SphereBaseMaterial;

	Ref<Material> m_MeshMaterial;
	std::vector<Ref<MaterialInstance>> m_MetalSphereMaterialInstances;
	std::vector<Ref<MaterialInstance>> m_DielectricSphereMaterialInstances;

	// PBR params
	bool m_RadiancePrefilter = false;
	float m_EnvMapRotation = 0.0f;

	// Editor resources
	std::map<std::string, Ref<Texture2D>> m_TexStore;

	int m_GizmoType = -1;
	bool m_AllowViewportCameraEvents = false;
	bool m_DrawOnTopBoundingBoxes = false;

	bool m_UIShowBoundingBoxes = false;
	bool m_UIShowBoundingBoxesOnTop = false;

	Scene::State m_SceneState = Scene::State::Edit;

	enum class SelectionMode
	{
		None = 0, Entity = 1, SubMesh = 2
	};

	SelectionMode m_SelectionMode = SelectionMode::Entity;
	std::vector<SelectedSubmesh> m_SelectionContext;
};

}
