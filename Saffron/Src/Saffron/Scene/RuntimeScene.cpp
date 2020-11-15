#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/GlobalTimer.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Scene/RuntimeScene.h"
#include "Saffron/Script/ScriptEngine.h"

namespace Se
{

template<typename T>
static void CopyComponent(EntityRegistry &dstRegistry, EntityRegistry &srcRegistry, const Map<UUID, Entity> &entityMap)
{
	auto components = srcRegistry.view<T>();
	for ( auto entityHandle : components )
	{
		Entity destEntity = entityMap.at(srcRegistry.get<IDComponent>(entityHandle).ID);

		auto &srcComponent = srcRegistry.get<T>(entityHandle);
		auto &destComponent = dstRegistry.emplace_or_replace<T>(destEntity.GetHandle(), srcComponent);
	}
}

RuntimeScene::RuntimeScene(Shared<Scene> copyFrom)
{
	SE_CORE_ASSERT(copyFrom, "Runtime Scene must have a scene to copy from");

	// Environment
	m_Light = copyFrom->GetLight();
	m_LightMultiplier = copyFrom->GetLight().Multiplier;

	m_Environment = copyFrom->GetEnvironment();
	m_Skybox = copyFrom->GetSkybox();
	m_SkyboxLod = copyFrom->GetSkyboxLod();

	auto &otherRegistry = copyFrom->GetEntityRegistry();
	Map<UUID, Entity> entityMap;
	auto idComponents = otherRegistry.view<IDComponent>();
	for ( auto entityHandle : idComponents )
	{
		auto uuid = otherRegistry.get<IDComponent>(entityHandle).ID;
		auto tag = otherRegistry.get<TagComponent>(entityHandle).Tag;
		const Entity entity = CreateEntity(uuid, tag);
		entityMap[uuid] = entity;
	}

	CopyComponent<TagComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<TransformComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<MeshComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<ScriptComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<CameraComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<SpriteRendererComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<RigidBody2DComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<BoxCollider2DComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<CircleCollider2DComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<RigidBody3DComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<BoxCollider3DComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<SphereCollider3DComponent>(m_EntityRegistry, otherRegistry, entityMap);

	const auto &entityInstanceMap = ScriptEngine::GetEntityInstanceMap();
	if ( entityInstanceMap.find(GetUUID()) != entityInstanceMap.end() )
	{
		ScriptEngine::CopyEntityScriptData(GetUUID(), copyFrom->GetUUID());
	}

	if ( copyFrom->GetEntity().HasComponent<PhysicsWorld2DComponent>() )
	{
		auto &copyFromWorld = copyFrom->GetEntity().GetComponent<PhysicsWorld2DComponent>().World;
		m_SceneEntity.GetComponent<PhysicsWorld2DComponent>().World.SetGravity(copyFromWorld.GetGravity());
	}
	if ( copyFrom->GetEntity().HasComponent<PhysicsWorld3DComponent>() )
	{
		auto &copyFromWorld = copyFrom->GetEntity().GetComponent<PhysicsWorld3DComponent>().World;
		m_SceneEntity.GetComponent<PhysicsWorld3DComponent>().World.SetGravity(copyFromWorld.GetGravity());
	}

	m_DebugCamera = m_SceneEntity.AddComponent<EditorCameraComponent>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)).Camera;
}

void RuntimeScene::OnUpdate()
{
	const auto ts = GlobalTimer::GetStep();

	auto view = m_EntityRegistry.view<ScriptComponent>();
	for ( auto entityHandle : view )
	{
		const UUID entityID = m_EntityRegistry.get<IDComponent>(entityHandle).ID;
		Entity entity = { entityHandle, this };
		if ( ScriptEngine::ModuleExists(entity.GetComponent<ScriptComponent>().ModuleName) )
		{
			ScriptEngine::OnUpdateEntity(m_SceneID, entityID, ts);
		}
	}


	if ( m_DebugCameraActivated && Input::IsKeyPressed(SE_KEY_LEFT_CONTROL) )
	{
		m_DebugCamera->OnUpdate();
	}

	if ( m_SceneEntity.HasComponent<PhysicsWorld2DComponent>() )
	{
		m_SceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnUpdate();
	}
	if ( m_SceneEntity.HasComponent<PhysicsWorld3DComponent>() )
	{
		m_SceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnUpdate();
	}
}

void RuntimeScene::OnRender()
{
	if ( m_DebugCameraActivated )
	{
		m_DebugCamera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		SceneRenderer::GetMainTarget()->SetCameraData({ Shared<Camera>::Cast(m_DebugCamera) , m_DebugCamera->GetViewMatrix() });
	}
	else
	{
		Entity cameraEntity = GetMainCameraEntity();
		if ( !cameraEntity )
		{
			SE_CORE_ASSERT(cameraEntity, "Scene does not contain any cameras!");
			return;
		}
		Shared<SceneCamera> camera = cameraEntity.GetComponent<CameraComponent>();
		camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		const Matrix4f cameraViewMatrix = glm::inverse(cameraEntity.GetComponent<TransformComponent>().Transform);
		SceneRenderer::GetMainTarget()->SetCameraData({ Shared<Camera>::Cast(camera) , cameraViewMatrix });
	}

	m_Skybox.Material->Set("u_TextureLod", m_SkyboxLod);

	SceneRenderer::BeginScene(this, { SceneRenderer::GetMainTarget() });
	auto group = m_EntityRegistry.group<MeshComponent>(entt::get<TransformComponent>);
	for ( auto entity : group )
	{
		auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
		if ( meshComponent.Mesh )
		{
			meshComponent.Mesh->OnUpdate();

			// TODO: Should we render (logically)
			SceneRenderer::SubmitMesh(meshComponent, transformComponent, nullptr);
		}
	}
	SceneRenderer::EndScene();

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

void RuntimeScene::OnGuiRender()
{
	ImGui::Begin("Scene");

	Gui::BeginPropertyGrid();
	Gui::Property("Debug Camera", m_DebugCameraActivated);
	Gui::Property("Load Environment Map", [this]
				  {
					  const Filepath filepath = FileIOManager::OpenFile({ "HDR Image (*.hdr)", {"*.hdr"} });
					  if ( !filepath.empty() )
					  {
						  SetEnvironment(Environment::Load(filepath.string()));
					  }
				  }, true);
	Gui::Property("Skybox LOD", GetSkyboxLod(), 0.0f, 11.0f, 0.5f, Gui::PropertyFlag::Drag);
	auto &light = GetLight();
	Gui::Property("Light Direction", light.Direction, Gui::PropertyFlag::Slider);
	Gui::Property("Light Radiance", light.Radiance, Gui::PropertyFlag::Color);
	Gui::Property("Light Multiplier", light.Multiplier, 0.0f, 5.0f, 0.25f, Gui::PropertyFlag::Slider);
	Gui::Property("Radiance Prefiltering", m_RadiancePrefilter);
	Gui::Property("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f, 0.5f, Gui::PropertyFlag::Slider);
	if ( Gui::Property("Show Bounding Boxes", m_UIShowBoundingBoxes) )
	{
		ShowBoundingBoxes(m_UIShowBoundingBoxes);
	}
	Gui::EndPropertyGrid();

	ImGui::End();

	if ( m_SceneEntity.HasComponent<PhysicsWorld2DComponent>() )
	{
		m_SceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnGuiRender();
	}
	if ( m_SceneEntity.HasComponent<PhysicsWorld3DComponent>() )
	{
		m_SceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnGuiRender();
	}

	m_DebugCamera->OnGuiRender();
}

void RuntimeScene::OnStart()
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

	if ( m_SceneEntity.HasComponent<PhysicsWorld2DComponent>() )
	{
		m_SceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnStart();
	}
	if ( m_SceneEntity.HasComponent<PhysicsWorld3DComponent>() )
	{
		m_SceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnStart();
	}
}

void RuntimeScene::OnStop()
{
	if ( m_SceneEntity.HasComponent<PhysicsWorld2DComponent>() )
	{
		m_SceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnStop();
	}
	if ( m_SceneEntity.HasComponent<PhysicsWorld3DComponent>() )
	{
		m_SceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnStop();
	}
}

void RuntimeScene::Escape()
{
	m_DebugCameraActivated = true;
	m_DebugCamera->Enable();
}

void RuntimeScene::Return()
{
	m_DebugCameraActivated = false;
	m_DebugCamera->Disable();
}
}
