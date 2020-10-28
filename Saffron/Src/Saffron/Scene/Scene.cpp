#include "SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Core/FileIOManager.h"
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
static void CopyComponent(entt::registry &dstRegistry, entt::registry &srcRegistry, const std::unordered_map<UUID, entt::entity> &enttMap)
{
	auto components = srcRegistry.view<T>();
	for ( auto srcEntity : components )
	{
		entt::entity destEntity = enttMap.at(srcRegistry.get<IDComponent>(srcEntity).ID);

		auto &srcComponent = srcRegistry.get<T>(srcEntity);
		auto &destComponent = dstRegistry.emplace_or_replace<T>(destEntity, srcComponent);
	}
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
	m_SceneEntity(m_EntityRegistry.create(), this)
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

void Scene::OnUpdate(Time ts)
{

	// Update all entities
	{
		auto view = m_EntityRegistry.view<ScriptComponent>();
		for ( auto entity : view )
		{
			const UUID entityID = m_EntityRegistry.get<IDComponent>(entity).ID;
			Entity e = { entity, this };
			if ( ScriptEngine::ModuleExists(e.GetComponent<ScriptComponent>().ModuleName) )
				ScriptEngine::OnUpdateEntity(m_SceneID, entityID, ts);
		}
	}

	if ( m_EntityRegistry.has<PhysicsWorld2DComponent>(m_SceneEntity) )
		m_EntityRegistry.get<PhysicsWorld2DComponent>(m_SceneEntity).World.OnUpdate();

}

void Scene::OnRenderRuntime(Time ts)
{
	/////////////////////////////////////////////////////////////////////
	// RENDER 3D SCENE
	/////////////////////////////////////////////////////////////////////
	Entity cameraEntity = GetMainCameraEntity();
	if ( !cameraEntity )
	{
		SE_CORE_ASSERT(cameraEntity, "Scene does not contain any cameras!");
		return;
	}

	Shared<SceneCamera> camera = cameraEntity.GetComponent<CameraComponent>();
	camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);

	m_Skybox.Material->Set("u_TextureLod", m_SkyboxLod);
	const Matrix4f cameraViewMatrix = glm::inverse(cameraEntity.GetComponent<TransformComponent>().Transform);

	auto group = m_EntityRegistry.group<MeshComponent>(entt::get<TransformComponent>);

	SceneRenderer::GetMainTarget()->SetCameraData({ static_cast<Camera *>(camera.Raw()), cameraViewMatrix });
	SceneRenderer::BeginScene(this);
	for ( auto entity : group )
	{
		auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
		if ( meshComponent.Mesh )
		{
			meshComponent.Mesh->OnUpdate(ts);

			// TODO: Should we render (logically)
			SceneRenderer::SubmitMesh(meshComponent, transformComponent, nullptr);
		}
	}
	SceneRenderer::EndScene();
	/////////////////////////////////////////////////////////////////////

#if 0
		// Render all sprites
	Renderer2D::BeginScene(*camera);
	{
		auto group = m_EntityRegistry.group<TransformComponent>(entt::get<SpriteRenderer>);
		for ( auto entity : group )
		{
			auto [transformComponent, spriteRendererComponent] = group.get<TransformComponent, SpriteRenderer>(entity);
			if ( spriteRendererComponent.Texture )
				Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Texture, spriteRendererComponent.TilingFactor);
			else
				Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Color);
		}
	}

	Renderer2D::EndScene();
#endif
}

void Scene::OnRenderEditor(Time ts, const EditorCamera &editorCamera)
{
	/////////////////////////////////////////////////////////////////////
	// RENDER 3D SCENE
	/////////////////////////////////////////////////////////////////////
	m_Skybox.Material->Set("u_TextureLod", m_SkyboxLod);

	auto group = m_EntityRegistry.group<MeshComponent>(entt::get<TransformComponent>);
	SceneRenderer::GetMainTarget()->SetCameraData({ static_cast<const Camera *>(&editorCamera), editorCamera.GetViewMatrix() });
	SceneRenderer::BeginScene(this);
	for ( auto entity : group )
	{
		auto [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
		if ( meshComponent.Mesh )
		{
			meshComponent.Mesh->OnUpdate(ts);

			// TODO: Should we render (logically)

			if ( m_SelectedEntity == entity )
				SceneRenderer::SubmitSelectedMesh(meshComponent, transformComponent);
			else
				SceneRenderer::SubmitMesh(meshComponent, transformComponent);
		}
	}
	SceneRenderer::EndScene();
	/////////////////////////////////////////////////////////////////////

#if 0
	// Render all sprites
	Renderer2D::BeginScene(*camera);
	{
		auto group = m_EntityRegistry.group<TransformComponent>(entt::get<SpriteRenderer>);
		for ( auto entity : group )
		{
			auto [transformComponent, spriteRendererComponent] = group.get<TransformComponent, SpriteRenderer>(entity);
			if ( spriteRendererComponent.Texture )
				Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Texture, spriteRendererComponent.TilingFactor);
			else
				Renderer2D::DrawQuad(transformComponent.Transform, spriteRendererComponent.Color);
		}
	}

	Renderer2D::EndScene();
#endif
}

void Scene::OnEvent(const Event &event)
{
}

void Scene::OnGuiRender()
{
	ImGui::Begin("Scene");

	if ( ImGui::Button("Load Environment Map") )
	{
		const Filepath filepath = FileIOManager::OpenFile({ "HDR Image (*.hdr)", {"*.hdr"} });
		if ( !filepath.empty() )
			SetEnvironment(Environment::Load(filepath.string()));
	}

	ImGui::SliderFloat("Skybox LOD", &GetSkyboxLod(), 0.0f, 11.0f);


	Gui::BeginPropertyGrid();

	auto &light = GetLight();
	Gui::Property("Light Direction", light.Direction, Gui::PropertyFlag::Slider);
	Gui::Property("Light Radiance", light.Radiance, Gui::PropertyFlag::Color);
	Gui::Property("Light Multiplier", light.Multiplier, 0.0f, 5.0f, 0.25f, Gui::PropertyFlag::Slider);
	//Gui::Property("Exposure", GetExposure(), 0.0f, 5.0f, Gui::PropertyFlag::Slider);
	Gui::Property("Radiance Prefiltering", m_RadiancePrefilter);
	Gui::Property("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f, 0.5f, Gui::PropertyFlag::Slider);
	if ( Gui::Property("Show Bounding Boxes", m_UIShowBoundingBoxes) )
		ShowBoundingBoxes(m_UIShowBoundingBoxes);
	Gui::EndPropertyGrid();


	ImGui::End();

	m_EntityRegistry.get<PhysicsWorld2DComponent>(m_SceneEntity).World.OnGuiRender();
}

void Scene::OnRuntimeStart()
{
	ScriptEngine::SetSceneContext(this);

	{
		auto view = m_EntityRegistry.view<ScriptComponent>();
		for ( auto eent : view )
		{
			Entity entity = { eent, this };
			auto component = entity.GetComponent<ScriptComponent>();
			if ( ScriptEngine::ModuleExists(component.ModuleName) )
			{
				ScriptEngine::InstantiateEntityClass(entity);
			}
		}
	}

	if ( m_EntityRegistry.has<PhysicsWorld2DComponent>(m_SceneEntity) )
		m_EntityRegistry.get<PhysicsWorld2DComponent>(m_SceneEntity).World.OnStart();



	m_IsPlaying = true;
}

void Scene::OnRuntimeStop()
{
	if ( m_EntityRegistry.has<PhysicsWorld2DComponent>(m_SceneEntity) )
		m_EntityRegistry.get<PhysicsWorld2DComponent>(m_SceneEntity).World.OnStop();

	m_IsPlaying = false;
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

Entity Scene::CreateEntityWithID(UUID uuid, const String &name, bool runtimeMap)
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

	m_EntityRegistry.destroy(entity.m_EntityHandle);
}

void Scene::DuplicateEntity(Entity entity)
{
	Entity newEntity = entity.HasComponent<TagComponent>() ? CreateEntity(entity.GetComponent<TagComponent>().Tag) : CreateEntity();

	CopyComponentIfExists<TransformComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_EntityRegistry);
	CopyComponentIfExists<MeshComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_EntityRegistry);
	CopyComponentIfExists<ScriptComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_EntityRegistry);
	CopyComponentIfExists<CameraComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_EntityRegistry);
	CopyComponentIfExists<SpriteRendererComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_EntityRegistry);
	CopyComponentIfExists<RigidBody2DComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_EntityRegistry);
	CopyComponentIfExists<BoxCollider2DComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_EntityRegistry);
	CopyComponentIfExists<CircleCollider2DComponent>(newEntity.m_EntityHandle, entity.m_EntityHandle, m_EntityRegistry);
}

Entity Scene::FindEntityByTag(const String &tag)
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

void Scene::CopyTo(Shared<Scene> &target)
{
	// Environment
	target->m_Light = m_Light;
	target->m_LightMultiplier = m_LightMultiplier;

	target->m_Environment = m_Environment;
	target->m_Skybox = m_Skybox;
	target->m_SkyboxLod = m_SkyboxLod;

	std::unordered_map<UUID, entt::entity> enttMap;
	auto idComponents = m_EntityRegistry.view<IDComponent>();
	for ( auto entity : idComponents )
	{
		auto uuid = m_EntityRegistry.get<IDComponent>(entity).ID;
		const Entity e = target->CreateEntityWithID(uuid, "", true);
		enttMap[uuid] = e.m_EntityHandle;
	}

	CopyComponent<TagComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);
	CopyComponent<TransformComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);
	CopyComponent<MeshComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);
	CopyComponent<ScriptComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);
	CopyComponent<CameraComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);
	CopyComponent<SpriteRendererComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);
	CopyComponent<RigidBody2DComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);
	CopyComponent<BoxCollider2DComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);
	CopyComponent<CircleCollider2DComponent>(target->m_EntityRegistry, m_EntityRegistry, enttMap);

	const auto &entityInstanceMap = ScriptEngine::GetEntityInstanceMap();
	if ( entityInstanceMap.find(target->GetUUID()) != entityInstanceMap.end() )
		ScriptEngine::CopyEntityScriptData(target->GetUUID(), m_SceneID);

	auto &world = m_EntityRegistry.get<PhysicsWorld2DComponent>(m_SceneEntity).World;
	target->GetEntityRegistry().get<PhysicsWorld2DComponent>(target->GetEntity()).World.SetGravity(world.GetGravity());
	/*if ( m_EntityRegistry.has<PhysicsWorld2DComponent>(m_SceneEntity) )
	{
		if ( !target->GetEntityRegistry().has<PhysicsWorld2DComponent>(target->GetEntity()) )

	}*/
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

	return {};
}

void Scene::SetName(String name)
{
	m_Name = Move(name);
}

void Scene::SetLight(const Light &light)
{
	m_Light = light;
}

void Scene::SetViewportSize(Uint32 width, Uint32 height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
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

void Scene::SetSelectedEntity(Entity entity)
{
	m_SelectedEntity = entity;
}

void Scene::ShowBoundingBoxes(bool show)
{
	SceneRenderer::GetOptions().ShowBoundingBoxes = show;
}
}
