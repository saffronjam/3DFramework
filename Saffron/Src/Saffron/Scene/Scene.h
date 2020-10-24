#pragma once

#include <unordered_map>

#include "Saffron/Core/Time.h"
#include "Saffron/Core/UUID.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Editor/EditorCamera.h"
#include "Saffron/Entity/EntityRegistry.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Renderer/Material.h"

namespace Se
{

class Entity;
using EntityMap = std::unordered_map<UUID, Entity>;

class Scene : public ReferenceCounted
{
public:
	struct Environment
	{
		std::string FilePath;
		Shared<TextureCube> RadianceMap;
		Shared<TextureCube> IrradianceMap;

		static Environment Load(const std::string &filepath);
	};

	struct Light
	{
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };

		float Multiplier = 1.0f;
	};

	struct Skybox
	{
		Shared<TextureCube> Texture;
		Shared<MaterialInstance> Material;
	};

public:
	explicit Scene(std::string name = "Scene");
	~Scene();

	void OnUpdate(Time ts);
	void OnRenderRuntime(Time ts);
	void OnRenderEditor(Time ts, const EditorCamera &editorCamera);
	void OnEvent(const Event &event);
	void OnGuiRender();
	void OnRuntimeStart();
	void OnRuntimeStop();

	Entity CreateEntity(std::string name = "");
	Entity CreateEntityWithID(UUID uuid, const std::string &name = "", bool runtimeMap = false);
	void DestroyEntity(Entity entity);

	template<typename T>
	auto GetAllEntitiesWith();
	void DuplicateEntity(Entity entity);
	Entity FindEntityByTag(const std::string &tag);
	const EntityMap &GetEntityMap() const { return m_EntityIDMap; }
	EntityRegistry &GetEntityRegistry() { return m_EntityRegistry; }
	const EntityRegistry &GetEntityRegistry() const { return m_EntityRegistry; }

	void CopyTo(Shared<Scene> &target);

	entt::entity GetEntity() const { return m_SceneEntity; }

	bool IsPlaying() const { return m_IsPlaying; }

	const std::string &GetName() const { return m_Name; }
	Light &GetLight() { return m_Light; }
	const Light &GetLight() const { return m_Light; }
	Entity GetMainCameraEntity();
	float &GetSkyboxLod() { return m_SkyboxLod; }
	UUID GetUUID() const { return m_SceneID; }
	static Shared<Scene> GetScene(UUID uuid);
	Skybox GetSkybox() const { return m_Skybox; }

	void SetName(std::string name);
	void SetLight(const Light &light);
	void SetViewportSize(Uint32 width, Uint32 height);
	void SetEnvironment(const Environment &environment);
	const Environment &GetEnvironment() const { return m_Environment; }
	void SetSkyboxTexture(const Shared<TextureCube> &skyboxTexture);

	// Editor-specific
	void SetSelectedEntity(Entity entity);

	void ShowBoundingBoxes(bool show);

private:
	UUID m_SceneID;
	std::string m_Name;

	Uint32 m_ViewportWidth = 0, m_ViewportHeight = 0;

	Light m_Light;
	float m_LightMultiplier = 0.3f;

	EntityRegistry m_EntityRegistry;
	EntityMap m_EntityIDMap;
	Entity m_SceneEntity;
	Entity m_SelectedEntity{};

	Environment m_Environment;
	Skybox m_Skybox;

	bool m_RadiancePrefilter = false;
	float m_EnvMapRotation = 0.0f;
	float m_SkyboxLod = 1.0f;
	bool m_IsPlaying = false;
	bool m_UIShowBoundingBoxes = false;

	friend void OnScriptComponentConstruct(entt::registry &registry, entt::entity entity);
	friend void OnScriptComponentDestroy(entt::registry &registry, entt::entity entity);
};

template <typename T>
auto Scene::GetAllEntitiesWith()
{
	return m_EntityRegistry.view<T>();
}
}

