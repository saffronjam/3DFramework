#pragma once

#include "Saffron/Base.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Entity/EntityRegistry.h"
#include "Saffron/Rendering/Resources/Texture.h"
#include "Saffron/Rendering/Material.h"
#include "Saffron/Scene/SceneComponents.h"

namespace Se
{
class Entity;
using EntityMap = std::unordered_map<UUID, Entity>;

struct Light
{
	Vector3 Direction = {0.0f, 0.0f, 0.0f};
	Vector3 Radiance = {0.0f, 0.0f, 0.0f};

	float Multiplier = 1.0f;
};

struct DirectionalLight
{
	Vector3 Direction = {0.0f, 0.0f, 0.0f};
	Vector3 Radiance = {0.0f, 0.0f, 0.0f};
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
	float Lod{};
	Shared<TextureCube> Texture;
	Shared<MaterialInstance> Material;
};

class Scene : public Managed
{
public:
	Scene();
	virtual ~Scene();

	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;

	virtual void OnEvent(const Event& event)
	{
	}

	virtual void OnGuiRender()
	{
	}

	Entity CreateEntity(String name);
	Entity CreateEntity(UUID uuid, const String& name);
	void DestroyEntity(Entity entity);
	Entity GetEntity(const String& tag);

	const EntityMap& GetEntityMap() const { return _entityIDMap; }

	EntityRegistry& GetEntityRegistry() { return _entityRegistry; }

	const EntityRegistry& GetEntityRegistry() const { return _entityRegistry; }

	Entity GetEntity() const { return _sceneEntity; }

	virtual Entity GetSelectedEntity() const { return _selectedEntity; }

	virtual void SetSelectedEntity(Entity entity) { _selectedEntity = entity; }

	virtual void SetViewportSize(uint width, uint height);

	UUID GetUUID() const { return _sceneID; }

	const String& GetName() const { return _name; }

	void SetName(String name) { _name = name; };

	Light& GetLight() { return _light; }

	const Light& GetLight() const { return _light; }

	void SetLight(const Light& light) { _light = light; }

	const LightEnvironment& GetLightEnvironment() const { return _lightEnvironment; }

	const Shared<SceneEnvironment>& GetSceneEnvironment() const;

	Entity GetMainCameraEntity();
	static Shared<Scene> GetScene(UUID uuid);

	const Skybox& GetSkybox() const { return _skybox; }

	void SetSkybox(const Skybox& skybox) { _skybox = skybox; }

	void SetSkybox(const Shared<TextureCube>& skybox);

	void ShowMeshBoundingBoxes(bool show);
	void ShowPhysicsBodyBoundingBoxes(bool show);

	static bool IsValidFilepath(const Filepath& filepath);

protected:
	UUID _sceneID;
	String _name;

	Light _light;
	float _lightMultiplier = 0.3f;
	LightEnvironment _lightEnvironment;
	Shared<SceneEnvironment> _sceneEnvironment;
	Shared<SceneEnvironment> _fallbackSceneEnvironment;
	Skybox _skybox;

	EntityRegistry _entityRegistry;
	EntityMap _entityIDMap;
	Entity _sceneEntity;
	Entity _selectedEntity;

	uint _viewportWidth = 0, _viewportHeight = 0;

	bool _radiancePrefilter = false;
	float _envMapRotation = 0.0f;
	bool _uIShowMeshBoundingBoxes = false;
	bool _uIShowPhysicsBodyBoundingBoxes = false;

private:
	friend void OnScriptComponentConstruct(EntityRegistry& registry, EntityHandle entity);
	friend void OnScriptComponentDestroy(EntityRegistry& registry, EntityHandle entity);
};
}
