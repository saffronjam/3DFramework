#pragma once

#include <unordered_map>

#include <entt/entt.hpp>

#include "Saffron/Core/Time.h"
#include "Saffron/Core/UUID.h"
#include "Saffron/Editor/EditorCamera.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Renderer/Material.h"


namespace Se
{
struct Environment
{
	std::string FilePath;
	Ref<TextureCube> RadianceMap;
	Ref<TextureCube> IrradianceMap;

	static Environment Load(const std::string &filepath);
};

struct Light
{
	glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };

	float Multiplier = 1.0f;
};

class Entity;
using EntityMap = std::unordered_map<UUID, Entity>;

class Scene : public RefCounted
{
public:
	enum class Type : Uint32
	{
		Spheres = 0, Model = 1
	};


public:
	Scene(std::string name = "Scene");
	~Scene();

	void Init();

	void OnUpdate(Time ts);
	void OnRenderRuntime(Time ts);
	void OnRenderEditor(Time ts, const EditorCamera &editorCamera);
	void OnEvent(const Event &event);

	// Runtime
	void OnRuntimeStart();
	void OnRuntimeStop();


	Entity CreateEntity(std::string name = "");
	Entity CreateEntityWithID(UUID uuid, const std::string &name = "", bool runtimeMap = false);
	void DestroyEntity(Entity entity);

	void DuplicateEntity(Entity entity);
	template<typename T>
	auto GetAllEntitiesWith();
	Entity FindEntityByTag(const std::string &tag);
	const EntityMap &GetEntityMap() const { return m_EntityIDMap; }

	void CopyTo(Ref<Scene> &target);

	const std::string &GetName() const { return m_Name; }
	Light &GetLight() { return m_Light; }
	const Light &GetLight() const { return m_Light; }
	Entity GetMainCameraEntity();
	float &GetSkyboxLod() { return m_SkyboxLod; }
	UUID GetUUID() const { return m_SceneID; }
	static Ref<Scene> GetScene(UUID uuid);
	float GetPhysics2DGravity() const;

	void SetName(std::string name);
	void SetLight(const Light &light);
	void SetViewportSize(Uint32 width, Uint32 height);
	void SetEnvironment(const Environment &environment);
	const Environment &GetEnvironment() const { return m_Environment; }
	void SetSkybox(const Ref<TextureCube> &skybox);
	void SetPhysics2DGravity(float gravity);

	// Editor-specific
	void SetSelectedEntity(entt::entity entity) { m_SelectedEntity = entity; }
private:
	UUID m_SceneID;
	std::string m_Name;

	Uint32 m_ViewportWidth = 0, m_ViewportHeight = 0;

	Light m_Light;
	float m_LightMultiplier = 0.3f;

	entt::entity m_SceneEntity;
	entt::registry m_Registry;
	entt::entity m_SelectedEntity{};
	EntityMap m_EntityIDMap;

	Environment m_Environment;
	Ref<TextureCube> m_SkyboxTexture;
	Ref<MaterialInstance> m_SkyboxMaterial;


	Entity *m_PhysicsBodyEntityBuffer = nullptr;

	float m_SkyboxLod = 1.0f;
	bool m_IsPlaying = false;

	friend class Entity;
	friend class SceneRenderer;
	friend class SceneSerializer;
	friend class SceneHierarchyPanel;

	friend void OnScriptComponentConstruct(entt::registry &registry, entt::entity entity);
	friend void OnScriptComponentDestroy(entt::registry &registry, entt::entity entity);
};

template <typename T>
auto Scene::GetAllEntitiesWith()
{
	return m_Registry.view<T>();
}
}

