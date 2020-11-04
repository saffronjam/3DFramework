#include "SaffronPCH.h"

#include <assimp/scene.h>

#include "Saffron/Core/Application.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Entity/EntityComponents.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Renderer/SceneRenderer.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Script/ScriptEngine.h"


namespace Se
{


///////////////////////////////////////////////////////////////////////////
/// Statics
///////////////////////////////////////////////////////////////////////////

std::unordered_map<UUID, Scene *> s_ActiveScenes;

///////////////////////////////////////////////////////////////////////////
/// Helper functions
///////////////////////////////////////////////////////////////////////////

static void OnScriptComponentConstruct(entt::registry &registry, entt::entity entity)
{
	const auto sceneView = registry.view<SceneComponent>();
	const UUID sceneID = registry.get<SceneComponent>(sceneView.front()).SceneID;

	Scene *scene = s_ActiveScenes[sceneID];

	const auto entityID = registry.get<IDComponent>(entity).ID;
	SE_CORE_ASSERT(scene->m_EntityIDMap.find(entityID) != scene->m_EntityIDMap.end());
	ScriptEngine::InitScriptEntity(scene->m_EntityIDMap.at(entityID));
}

static void OnScriptComponentDestroy(entt::registry &registry, entt::entity entity)
{
	const auto sceneView = registry.view<SceneComponent>();
	const UUID sceneID = registry.get<SceneComponent>(sceneView.front()).SceneID;

	[[maybe_unused]] Scene *scene = s_ActiveScenes[sceneID];

	const auto entityID = registry.get<IDComponent>(entity).ID;
	ScriptEngine::OnScriptComponentDestroyed(sceneID, entityID);
}

template<typename T>
static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry &registry)
{
	if ( registry.has<T>(src) )
	{
		auto &srcComponent = registry.get<T>(src);
		registry.emplace_or_replace<T>(dst, srcComponent);
	}
}



///////////////////////////////////////////////////////////////////////////
/// Environment
///////////////////////////////////////////////////////////////////////////

Scene::Environment Scene::Environment::Load(const String &filepath)
{
	auto [radiance, irradiance] = SceneRenderer::CreateEnvironmentMap(filepath);
	return { filepath, radiance, irradiance };
}


///////////////////////////////////////////////////////////////////////////
/// Scene
///////////////////////////////////////////////////////////////////////////

Scene::Scene(String name)
	: m_Name(Move(name)),
	m_SceneEntity(m_EntityRegistry.create(), this),
	m_ViewportWidth(100),
	m_ViewportHeight(100)
{
	m_EntityRegistry.on_construct<ScriptComponent>().connect<&OnScriptComponentConstruct>();
	m_EntityRegistry.on_destroy<ScriptComponent>().connect<&OnScriptComponentDestroy>();

	m_EntityRegistry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);
	m_EntityRegistry.emplace<PhysicsWorld2DComponent>(m_SceneEntity, *this);

	s_ActiveScenes[m_SceneID] = this;

	const auto skyboxShader = Shader::Create(Filepath{ "Assets/shaders/Skybox.glsl" });
	m_Skybox.Material = MaterialInstance::Create(Material::Create(skyboxShader));
	m_Skybox.Material->SetFlag(Material::Flag::DepthTest, false);
}

Scene::~Scene()
{
	m_EntityRegistry.on_destroy<ScriptComponent>().disconnect();
	m_EntityRegistry.clear();

	s_ActiveScenes.erase(m_SceneID);
	ScriptEngine::OnSceneDestruct(m_SceneID);
}

Entity Scene::CreateEntity(String name)
{
	auto entity = Entity{ m_EntityRegistry.create(), this };
	auto &idComponent = entity.AddComponent<IDComponent>();
	idComponent.ID = {};

	entity.AddComponent<TransformComponent>(Matrix4f(1.0f));
	if ( !name.empty() )
		entity.AddComponent<TagComponent>(Move(name));

	m_EntityIDMap[idComponent.ID] = entity;
	return entity;
}

Entity Scene::CreateEntity(UUID uuid, const String &name)
{
	auto entity = Entity{ m_EntityRegistry.create(), this };
	auto &idComponent = entity.AddComponent<IDComponent>();
	idComponent.ID = uuid;

	entity.AddComponent<TransformComponent>(Matrix4f(1.0f));
	if ( !name.empty() )
		entity.AddComponent<TagComponent>(name);

	SE_CORE_ASSERT(m_EntityIDMap.find(uuid) == m_EntityIDMap.end());
	m_EntityIDMap[uuid] = entity;
	return entity;
}

void Scene::DestroyEntity(Entity entity)
{
	if ( entity.HasComponent<ScriptComponent>() )
		ScriptEngine::OnScriptComponentDestroyed(m_SceneID, entity.GetUUID());

	m_EntityRegistry.destroy(entity.GetHandle());
}

Entity Scene::GetEntity(const String &tag)
{
	// TODO: If this becomes used often, consider indexing by tag
	auto view = m_EntityRegistry.view<TagComponent>();
	for ( auto entity : view )
	{
		const auto &candidate = view.get<TagComponent>(entity).Tag;
		if ( candidate == tag )
			return Entity(entity, this);
	}

	return Entity::Null();
}

void Scene::DuplicateEntity(Entity entity)
{
	const Entity newEntity = entity.HasComponent<TagComponent>() ? CreateEntity(entity.GetComponent<TagComponent>().Tag) : CreateEntity("Unnamed");

	const auto otherEntityHandle = entity.GetHandle();
	const auto newEntityHandle = newEntity.GetHandle();

	CopyComponentIfExists<TransformComponent>(newEntityHandle, otherEntityHandle, m_EntityRegistry);
	CopyComponentIfExists<MeshComponent>(newEntityHandle, otherEntityHandle, m_EntityRegistry);
	CopyComponentIfExists<ScriptComponent>(newEntityHandle, otherEntityHandle, m_EntityRegistry);
	CopyComponentIfExists<CameraComponent>(newEntityHandle, otherEntityHandle, m_EntityRegistry);
	CopyComponentIfExists<SpriteRendererComponent>(newEntityHandle, otherEntityHandle, m_EntityRegistry);
	CopyComponentIfExists<RigidBody2DComponent>(newEntityHandle, otherEntityHandle, m_EntityRegistry);
	CopyComponentIfExists<BoxCollider2DComponent>(newEntityHandle, otherEntityHandle, m_EntityRegistry);
	CopyComponentIfExists<CircleCollider2DComponent>(newEntityHandle, otherEntityHandle, m_EntityRegistry);
}

void Scene::SetSelectedEntity(Entity entity)
{
	m_SelectedEntity = entity;
}

Entity Scene::GetSelectedEntity()
{
	return m_SelectedEntity;
}

void Scene::SetViewportSize(Uint32 width, Uint32 height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
}

Entity Scene::GetMainCameraEntity()
{
	auto view = m_EntityRegistry.view<CameraComponent>();
	for ( auto entity : view )
	{
		auto &comp = view.get<CameraComponent>(entity);
		if ( comp.Primary )
			return { entity, this };
	}
	return Entity::Null();
}

Shared<Scene> Scene::GetScene(UUID uuid)
{
	if ( s_ActiveScenes.find(uuid) != s_ActiveScenes.end() )
		return s_ActiveScenes.at(uuid);

	return nullptr;
}

void Scene::SetName(String name)
{
	m_Name = Move(name);
}

void Scene::SetLight(const Light &light)
{
	m_Light = light;
}

void Scene::SetEnvironment(const Environment &environment)
{
	m_Environment = environment;
	SetSkyboxTexture(environment.RadianceMap);
}

void Scene::SetSkyboxTexture(const Shared<TextureCube> &skyboxTexture)
{
	m_Skybox.Texture = skyboxTexture;
	m_Skybox.Material->Set("u_Texture", skyboxTexture);
}

void Scene::ShowBoundingBoxes(bool show)
{
	SceneRenderer::GetOptions().ShowBoundingBoxes = show;
}
}
