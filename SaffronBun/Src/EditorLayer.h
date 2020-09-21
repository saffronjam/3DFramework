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
	enum class PropertyFlag
	{
		None = 0, ColorProperty = 1, DragProperty = 2, SliderProperty = 4
	};
public:
	EditorLayer();
	virtual ~EditorLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Time ts) override;

	void OnImGuiRender() override;
	void OnEvent(const Event &event) override;
	bool OnKeyboardPressEvent(const KeyboardPressEvent &event);
	bool OnMouseButtonPressed(const MousePressEvent &event);

	// ImGui UI helpers
	bool Property(const std::string &name, bool &value);
	bool Property(const std::string &name, float &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None) const;
	bool Property(const std::string &name, glm::vec2 &value, PropertyFlag flags) const;
	bool Property(const std::string &name, glm::vec2 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None) const;
	bool Property(const std::string &name, glm::vec3 &value, PropertyFlag flags) const;
	bool Property(const std::string &name, glm::vec3 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None) const;
	bool Property(const std::string &name, glm::vec4 &value, PropertyFlag flags) const;
	bool Property(const std::string &name, glm::vec4 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None) const;

	void ShowBoundingBoxes(bool show, bool onTop = false);
	void SelectEntity(Entity entity);

	void OpenScene();
	void SaveScene() const;
	void SaveSceneAs();
private:
	glm::vec2 GetMouseViewportSpace() const;
	std::pair<glm::vec3, glm::vec3> CastRay(float mx, float my) const;

	struct SelectedSubmesh
	{
		Entity Entity;
		Submesh *Mesh = nullptr;
		float Distance = 0.0f;
	};

	void OnSelected(const SelectedSubmesh &selectionContext);
	void OnEntityDeleted(Entity e);
	Ray CastMouseRay() const;

	void OnScenePlay();
	void OnSceneStop();

	void UpdateWindowTitle(const std::string &sceneName);

	float GetSnapValue() const;
private:
	Scope<SceneHierarchyPanel> m_SceneHierarchyPanel;

	Ref<Scene> m_RuntimeScene, m_EditorScene;
	std::string m_SceneFilePath;
	bool m_ReloadScriptOnPlay = true;

	EditorCamera m_EditorCamera;

	Ref<Shader> m_BrushShader;
	Ref<Material> m_SphereBaseMaterial;

	Ref<Material> m_MeshMaterial;
	std::vector<Ref<MaterialInstance>> m_MetalSphereMaterialInstances;
	std::vector<Ref<MaterialInstance>> m_DielectricSphereMaterialInstances;

	struct AlbedoInput
	{
		glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; // Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
		Ref<Texture2D> TextureMap;
		bool sRGB = true;
		bool UseTexture = false;
	};
	//AlbedoInput m_AlbedoInput;

	struct NormalInput
	{
		Ref<Texture2D> TextureMap;
		bool UseTexture = false;
	};
	//NormalInput m_NormalInput;

	struct MetalnessInput
	{
		float Value = 1.0f;
		Ref<Texture2D> TextureMap;
		bool UseTexture = false;
	};
	//MetalnessInput m_MetalnessInput;

	struct RoughnessInput
	{
		float Value = 0.2f;
		Ref<Texture2D> TextureMap;
		bool UseTexture = false;
	};
	//RoughnessInput m_RoughnessInput;

	// PBR params
	bool m_RadiancePrefilter = false;

	float m_EnvMapRotation = 0.0f;

	enum class SceneType : uint32_t
	{
		Spheres = 0, Model = 1
	};
	SceneType m_SceneType;

	// Editor resources
	Ref<Texture2D> m_CheckerboardTex;
	Ref<Texture2D> m_PlayButtonTex;

	glm::vec2 m_ViewportBounds[2];
	int m_GizmoType = -1; // -1 = no gizmo
	float m_SnapValue = 0.5f;
	float m_RotationSnapValue = 45.0f;
	bool m_AllowViewportCameraEvents = false;
	bool m_DrawOnTopBoundingBoxes = false;

	bool m_UIShowBoundingBoxes = false;
	bool m_UIShowBoundingBoxesOnTop = false;

	bool m_ViewportPanelMouseOver = false;
	bool m_ViewportPanelFocused = false;

	enum class SceneState
	{
		Edit = 0, Play = 1, Pause = 2
	};
	SceneState m_SceneState = SceneState::Edit;

	enum class SelectionMode
	{
		None = 0, Entity = 1, SubMesh = 2
	};

	SelectionMode m_SelectionMode = SelectionMode::Entity;
	std::vector<SelectedSubmesh> m_SelectionContext;
	glm::mat4 *m_RelativeTransform = nullptr;
	glm::mat4 *m_CurrentlySelectedTransform = nullptr;
};

}
