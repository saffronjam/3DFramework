#pragma once

#include <unordered_map>

#include "Saffron/Editor/ViewportPane.h"

#include "Saffron/Core/UUID.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Entity/EntityRegistry.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Renderer/Material.h"
#include "Saffron/Renderer/SceneRenderer.h"

namespace Se
{

class Entity;
using EntityMap = std::unordered_map<UUID, Entity>;

class Scene : public ReferenceCounted
{
public:
	struct Environment
	{
		String FilePath;
		Shared<TextureCube> RadianceMap;
		Shared<TextureCube> IrradianceMap;

		static Environment Load(const String &filepath);
	};

	struct Light
	{
		Vector3f Direction = { 0.0f, 0.0f, 0.0f };
		Vector3f Radiance = { 0.0f, 0.0f, 0.0f };

		float Multiplier = 1.0f;
	};

	struct Skybox
	{
		Shared<TextureCube> Texture;
		Shared<MaterialInstance> Material;
	};

public:
	explicit Scene(String name);
	~Scene();

	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnEvent(const Event &event) {}
	virtual void OnGuiRender() {}

	Entity CreateEntity(String name);
	Entity CreateEntity(UUID uuid, const String &name);
	void DestroyEntity(Entity entity);
	Entity GetEntity(const String &tag);

	void DuplicateEntity(Entity entity);

	const EntityMap &GetEntityMap() const { return m_EntityIDMap; }
	EntityRegistry &GetEntityRegistry() { return m_EntityRegistry; }
	const EntityRegistry &GetEntityRegistry() const { return m_EntityRegistry; }

	virtual const Shared<SceneRenderer::Target> &GetTarget() const = 0;

	virtual Entity GetSelectedEntity();
	virtual	void SetSelectedEntity(Entity entity);
	virtual void SetViewportSize(Uint32 width, Uint32 height);

	Entity GetEntity() const { return m_SceneEntity; }
	const String &GetName() const { return m_Name; }
	Light &GetLight() { return m_Light; }
	const Light &GetLight() const { return m_Light; }
	Entity GetMainCameraEntity();
	float &GetSkyboxLod() { return m_SkyboxLod; }
	UUID GetUUID() const { return m_SceneID; }
	static Shared<Scene> GetScene(UUID uuid);
	Skybox GetSkybox() const { return m_Skybox; }
	const Environment &GetEnvironment() const { return m_Environment; }

	void SetName(String name);
	void SetLight(const Light &light);
	void SetEnvironment(const Environment &environment);
	void SetSkyboxTexture(const Shared<TextureCube> &skyboxTexture);
	void ShowBoundingBoxes(bool show);

protected:
	UUID m_SceneID;
	String m_Name;

	Light m_Light;
	float m_LightMultiplier = 0.3f;

	EntityRegistry m_EntityRegistry;
	EntityMap m_EntityIDMap;
	Entity m_SceneEntity;
	Entity m_SelectedEntity;

	Uint32 m_ViewportWidth = 0, m_ViewportHeight = 0;

	Environment m_Environment;
	Skybox m_Skybox;

	bool m_RadiancePrefilter = false;
	float m_EnvMapRotation = 0.0f;
	float m_SkyboxLod = 1.0f;
	bool m_UIShowBoundingBoxes = false;

private:
	friend void OnScriptComponentConstruct(entt::registry &registry, entt::entity entity);
	friend void OnScriptComponentDestroy(entt::registry &registry, entt::entity entity);
};
}

