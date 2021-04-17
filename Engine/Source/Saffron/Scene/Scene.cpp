#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Entity/EntityComponents.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Script/ScriptEngine.h"


namespace Se
{
///////////////////////////////////////////////////////////////////////////
/// Statics
///////////////////////////////////////////////////////////////////////////

UnorderedMap<UUID, Scene*> s_ActiveScenes;

///////////////////////////////////////////////////////////////////////////
/// Helper functions
///////////////////////////////////////////////////////////////////////////

void OnScriptComponentConstruct(EntityRegistry& registry, EntityHandle entity)
{
	const auto sceneView = registry.view<SceneIDComponent>();
	const UUID sceneID = registry.get<SceneIDComponent>(sceneView.front()).SceneID;

	Scene* scene = s_ActiveScenes[sceneID];

	const auto entityID = registry.get<IDComponent>(entity).ID;
	SE_CORE_ASSERT(scene->_entityIDMap.find(entityID) != scene->_entityIDMap.end());
	ScriptEngine::InitScriptEntity(scene->_entityIDMap.at(entityID));
}

void OnScriptComponentDestroy(EntityRegistry& registry, EntityHandle entity)
{
	const auto sceneView = registry.view<SceneIDComponent>();
	const UUID sceneID = registry.get<SceneIDComponent>(sceneView.front()).SceneID;

	[[maybe_unused]] Scene* scene = s_ActiveScenes[sceneID];

	const auto entityID = registry.get<IDComponent>(entity).ID;
	ScriptEngine::OnScriptComponentDestroyed(sceneID, entityID);
}

template <typename T>
static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
{
	if (registry.has<T>(src))
	{
		auto& srcComponent = registry.get<T>(src);
		registry.emplace_or_replace<T>(dst, srcComponent);
	}
}


///////////////////////////////////////////////////////////////////////////
/// Scene
///////////////////////////////////////////////////////////////////////////

Scene::Scene() :
	_fallbackSceneEnvironment(SceneEnvironment::Load("pink_sunrise_4k.hdr")),
	_sceneEntity(_entityRegistry.create(), this),
	_viewportWidth(100),
	_viewportHeight(100)
{
	_entityRegistry.on_construct<ScriptComponent>().connect<&OnScriptComponentConstruct>();
	_entityRegistry.on_destroy<ScriptComponent>().connect<&OnScriptComponentDestroy>();

	_sceneEntity.AddComponent<SceneIDComponent>(_sceneID);
	_sceneEntity.AddComponent<PhysicsWorld2DComponent>(*this);
	_sceneEntity.AddComponent<PhysicsWorld3DComponent>(*this);

	s_ActiveScenes[_sceneID] = this;

	const auto skyboxShader = Shader::Create("Skybox");
	_skybox.Material = MaterialInstance::Create(Shared<Material>::Create(skyboxShader));
	_skybox.Material->SetFlag(MaterialFlag::DepthTest, false);
}

Scene::~Scene()
{
	if (_sceneEntity.HasComponent<PhysicsWorld2DComponent>())
	{
		_sceneEntity.RemoveComponent<PhysicsWorld2DComponent>();
	}
	if (_sceneEntity.HasComponent<PhysicsWorld3DComponent>())
	{
		_sceneEntity.RemoveComponent<PhysicsWorld3DComponent>();
	}

	_entityRegistry.on_destroy<ScriptComponent>().disconnect();
	_entityRegistry.clear();

	s_ActiveScenes.erase(_sceneID);
}

Entity Scene::CreateEntity(String name)
{
	auto entity = Entity{_entityRegistry.create(), this};
	auto& idComponent = entity.AddComponent<IDComponent>();
	idComponent.ID = {};

	entity.AddComponent<TransformComponent>(Matrix4f(1.0f));
	if (!name.empty()) entity.AddComponent<TagComponent>(Move(name));

	_entityIDMap[idComponent.ID] = entity;
	return entity;
}

Entity Scene::CreateEntity(UUID uuid, const String& name)
{
	auto entity = Entity{_entityRegistry.create(), this};
	auto& idComponent = entity.AddComponent<IDComponent>();
	idComponent.ID = uuid;

	entity.AddComponent<TransformComponent>(Matrix4f(1.0f));
	if (!name.empty()) entity.AddComponent<TagComponent>(name);

	SE_CORE_ASSERT(_entityIDMap.find(uuid) == _entityIDMap.end());
	_entityIDMap[uuid] = entity;
	return entity;
}

void Scene::DestroyEntity(Entity entity)
{
	if (entity.HasComponent<ScriptComponent>()) ScriptEngine::OnScriptComponentDestroyed(_sceneID, entity.GetUUID());

	_entityRegistry.destroy(entity.GetHandle());
}

Entity Scene::GetEntity(const String& tag)
{
	// TODO: If this becomes used often, consider indexing by tag
	auto view = _entityRegistry.view<TagComponent>();
	for (auto entity : view)
	{
		const auto& candidate = view.get<TagComponent>(entity).Tag;
		if (candidate == tag) return Entity(entity, this);
	}

	return Entity::Null();
}

void Scene::SetViewportSize(Uint32 width, Uint32 height)
{
	_viewportWidth = width;
	_viewportHeight = height;
}

const Shared<SceneEnvironment>& Scene::GetSceneEnvironment() const
{
	return _sceneEnvironment ? _sceneEnvironment : _fallbackSceneEnvironment;
}

Entity Scene::GetMainCameraEntity()
{
	auto view = _entityRegistry.view<CameraComponent>();
	for (auto entity : view)
	{
		auto& comp = view.get<CameraComponent>(entity);
		if (comp.Primary) return {entity, this};
	}
	return Entity::Null();
}

Shared<Scene> Scene::GetScene(UUID uuid)
{
	if (s_ActiveScenes.find(uuid) != s_ActiveScenes.end()) return s_ActiveScenes.at(uuid);

	return nullptr;
}

void Scene::SetSkybox(const Shared<TextureCube>& skybox)
{
	_skybox.Texture = skybox;
	_skybox.Material->Set("u_Texture", skybox);
}

void Scene::ShowMeshBoundingBoxes(bool show)
{
	SceneRenderer::GetOptions().ShowBoundingBoxes = show;
}

void Scene::ShowPhysicsBodyBoundingBoxes(bool show)
{
	//SceneRenderer::GetOptions().ShowPhysicsBodyBoundingBoxes = show;
}

bool Scene::IsValidFilepath(const Filepath& filepath)
{
	return !filepath.empty() && filepath.extension() == ".ssc" && FileIOManager::FileExists(filepath);
}
}
