#pragma once

#include "Saffron/Editor/ViewportPane.h"

#include "Saffron/Base.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Entity/EntityRegistry.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Renderer/Material.h"
#include "Saffron/Renderer/SceneRenderer.h"
#include "Saffron/Scene/SceneEnvironment.h"
#include "Saffron/Scene/SceneComponents.h"

namespace Se
{

class Entity;
using EntityMap = std::unordered_map<UUID, Entity>;

class Scene : public MemManaged<Scene>
{
public:
	struct Light
	{
		Vector3f Direction = { 0.0f, 0.0f, 0.0f };
		Vector3f Radiance = { 0.0f, 0.0f, 0.0f };

		float Multiplier = 1.0f;
	};

	struct DirectionalLight
	{
		Vector3f Direction = { 0.0f, 0.0f, 0.0f };
		Vector3f Radiance = { 0.0f, 0.0f, 0.0f };
		float Multiplier = 0.0f;

		// C++ only
		bool CastShadows = true;
	};

	struct LightEnvironment
	{
		DirectionalLight DirectionalLights[4];
	};

	struct Skybox
	{
		std::shared_ptr<TextureCube> Texture;
		std::shared_ptr<MaterialInstance> Material;
	};

public:
	explicit Scene();
	~Scene();

	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnEvent(const Event &event) {}
	virtual void OnGuiRender() {}

	Entity CreateEntity(String name);
	Entity CreateEntity(UUID uuid, const String &name);
	void DestroyEntity(Entity entity);
	Entity GetEntity(const String &tag);

	const EntityMap &GetEntityMap() const { return m_EntityIDMap; }
	EntityRegistry &GetEntityRegistry() { return m_EntityRegistry; }
	const EntityRegistry &GetEntityRegistry() const { return m_EntityRegistry; }

	virtual const std::shared_ptr<SceneRenderer::Target> &GetTarget() const = 0;

	virtual Entity GetSelectedEntity();
	virtual	void SetSelectedEntity(Entity entity);
	virtual void SetViewportSize(Uint32 width, Uint32 height);

	UUID GetUUID() const { return m_SceneID; }
	const String &GetName() const { return m_Name; }

	Entity GetEntity() const { return m_SceneEntity; }

	Light &GetLight() { return m_Light; }
	const Light &GetLight() const { return m_Light; }
	const LightEnvironment &GetLightEnvironment() const { return m_LightEnvironment; }

	Entity GetMainCameraEntity();
	float &GetSkyboxLod() { return m_SkyboxLod; }
	static std::shared_ptr<Scene> GetScene(UUID uuid);
	Skybox GetSkybox() const { return m_Skybox; }
	const std::shared_ptr<SceneEnvironment> &GetEnvironment() const { return m_Environment; }

	void SetName(String name);
	void SetLight(const Light &light);
	void SetEnvironment(const std::shared_ptr<SceneEnvironment> &environment);
	void SetSkyboxTexture(const std::shared_ptr<TextureCube> &skyboxTexture);
	void ShowMeshBoundingBoxes(bool show);
	void ShowPhysicsBodyBoundingBoxes(bool show);

	static bool IsValidFilepath(const Filepath &filepath);

protected:
	UUID m_SceneID;
	String m_Name;

	Light m_Light;
	float m_LightMultiplier = 0.3f;
	LightEnvironment m_LightEnvironment;

	EntityRegistry m_EntityRegistry;
	EntityMap m_EntityIDMap;
	Entity m_SceneEntity;
	Entity m_SelectedEntity;

	Uint32 m_ViewportWidth = 0, m_ViewportHeight = 0;

	std::shared_ptr<SceneEnvironment> m_Environment;
	Skybox m_Skybox;

	bool m_RadiancePrefilter = false;
	float m_EnvMapRotation = 0.0f;
	float m_SkyboxLod = 1.0f;
	bool m_UIShowMeshBoundingBoxes = false;
	bool m_UIShowPhysicsBodyBoundingBoxes = false;

private:
	friend void OnScriptComponentConstruct(EntityRegistry &registry, EntityHandle entity);
	friend void OnScriptComponentDestroy(EntityRegistry &registry, EntityHandle entity);
};
}

