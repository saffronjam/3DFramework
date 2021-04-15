#include "SaffronPCH.h"

#include "Saffron/Core/Misc.h"
#include "Saffron/Core/Global.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Keyboard.h"
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

	m_SceneEnvironment = copyFrom->GetSceneEnvironment();
	m_Skybox = copyFrom->GetSkybox();

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
	CopyComponent<CameraComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<DirectionalLightComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<SkylightComponent>(m_EntityRegistry, otherRegistry, entityMap);
	CopyComponent<ScriptComponent>(m_EntityRegistry, otherRegistry, entityMap);
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
	const auto ts = Global::Timer::GetStep();

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


	if ( m_DebugCameraActivated && Keyboard::IsPressed(KeyCode::LControl) )
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
	// Process lights
	{
		m_LightEnvironment = LightEnvironment();
		auto lights = m_EntityRegistry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
		Uint32 directionalLightIndex = 0;
		for ( auto entity : lights )
		{
			auto [transformComponent, lightComponent] = lights.get<TransformComponent, DirectionalLightComponent>(entity);
			Vector3f direction = -glm::normalize(Matrix3f(transformComponent.Transform) * Vector3f(1.0f));
			m_LightEnvironment.DirectionalLights[directionalLightIndex++] =
			{
				direction,
				lightComponent.Radiance,
				lightComponent.Intensity,
				lightComponent.CastShadows
			};
		}
	}

	// TODO: only one sky light at the moment!
	{
		auto lights = m_EntityRegistry.group<SkylightComponent>(entt::get<TransformComponent>);
		for ( auto entity : lights )
		{
			auto [transformComponent, skyLightComponent] = lights.get<TransformComponent, SkylightComponent>(entity);
			m_SceneEnvironment = skyLightComponent.SceneEnvironment;
			m_SceneEnvironment->SetIntensity(skyLightComponent.Intensity);
			SetSkybox(m_SceneEnvironment->GetRadianceMap());
		}
	}

	if ( m_DebugCameraActivated )
	{
		m_DebugCamera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		SceneRenderer::GetMainTarget()->SetCameraData({ std::dynamic_pointer_cast<Camera>(m_DebugCamera) , m_DebugCamera->GetViewMatrix(), 0.1f, 1000.0f, 45.0f });
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
		SceneRenderer::GetMainTarget()->SetCameraData({ std::dynamic_pointer_cast<Camera>(camera) , cameraViewMatrix, 0.1f, 1000.0f, 45.0f });
	}

	m_Skybox.Material->Set("u_TextureLod", m_Skybox.Lod);

	SceneRenderer::BeginScene(this, { SceneRenderer::GetMainTarget() });
	auto meshGroup = m_EntityRegistry.group<MeshComponent>(entt::get<TransformComponent>);
	for ( const auto &entityHandle : meshGroup )
	{
		auto [meshComponent, transformComponent] = meshGroup.get<MeshComponent, TransformComponent>(entityHandle);
		if ( meshComponent.Mesh )
		{
			meshComponent.Mesh->OnUpdate();

			// TODO: Should we render (logically)
			SceneRenderer::SubmitMesh(meshComponent, transformComponent);
		}
	}
	auto cameraGroup = m_EntityRegistry.group<CameraComponent>(entt::get<TransformComponent>);
	for ( const auto &entityHandle : cameraGroup )
	{
		auto [cameraComponent, transformComponent] = cameraGroup.get<CameraComponent, TransformComponent>(entityHandle);
		if ( cameraComponent.DrawMesh && cameraComponent.CameraMesh )
		{
			m_SelectedEntity == entityHandle ? SceneRenderer::SubmitSelectedMesh(cameraComponent.CameraMesh, transformComponent) : SceneRenderer::SubmitMesh(cameraComponent.CameraMesh, transformComponent);
		}
		if ( cameraComponent.DrawFrustum )
		{
			cameraComponent.Camera->RenderFrustum(transformComponent.Transform);
		}
	}

	static auto render2DPhysicsBoundingBox = [this](Entity entity)
	{
		// TODO: Implement
	};

	static auto constexpr render3DPhysicsBoundingBox = [](Entity entity)
	{
		const auto &boxComponent = entity.GetComponent<BoxCollider3DComponent>();
		const auto &[translation, rotation, scale] = Misc::GetTransformDecomposition(entity.GetComponent<TransformComponent>().Transform);
		const auto &boxSize = boxComponent.Size;
		const auto &boxOffset = boxComponent.Offset;
		const Matrix4f transformation = Math::ComposeMatrix(translation + boxOffset, rotation, boxSize / 2.0f);
		SceneRenderer::SubmitAABB(AABB{ Vector3f{-1.0f}, Vector3f{1.0f} }, transformation, { 0.89f, 0.46f, 0.16f, 1.0f });
	};

	if ( SceneRenderer::GetOptions().ShowPhysicsBodyBoundingBoxes )
	{
		if ( m_SceneEntity.HasComponent<PhysicsWorld2DComponent>() )
		{
			auto view = m_EntityRegistry.view<TransformComponent, BoxCollider2DComponent>();
			for ( auto entityHandle : view )
			{
				const Entity entity = { entityHandle, this };
				render2DPhysicsBoundingBox(entity);
			}
		}
		if ( m_SceneEntity.HasComponent<PhysicsWorld3DComponent>() )
		{
			auto view = m_EntityRegistry.view<TransformComponent, BoxCollider3DComponent>();
			for ( auto entityHandle : view )
			{
				const Entity entity = { entityHandle, this };
				render3DPhysicsBoundingBox(entity);
			}
		}
	}
	else
	{
		auto boxCollider2DView = m_EntityRegistry.view<BoxCollider2DComponent>();
		for ( const auto &entityHandle : boxCollider2DView )
		{
			if ( boxCollider2DView.get<BoxCollider2DComponent>(entityHandle).DrawBounding )
			{
				const Entity entity = { entityHandle, this };
				render2DPhysicsBoundingBox(entity);
			}
		}
		auto boxCollider3DView = m_EntityRegistry.view<BoxCollider3DComponent>();
		for ( const auto &entityHandle : boxCollider3DView )
		{
			if ( boxCollider3DView.get<BoxCollider3DComponent>(entityHandle).DrawBounding )
			{
				const Entity entity = { entityHandle, this };
				render3DPhysicsBoundingBox(entity);
			}
		}
	}
	SceneRenderer::EndScene();
}

void RuntimeScene::OnGuiRender()
{
	ImGui::Begin("Scene");

	Gui::BeginPropertyGrid();
	Gui::Property("Debug Camera", m_DebugCameraActivated);
	Gui::Property("Skybox LOD", m_Skybox.Lod, 0.0f, 11.0f, 0.5f, Gui::PropertyFlag::Drag);
	auto &light = GetLight();
	Gui::Property("Light Direction", light.Direction, Gui::PropertyFlag::Slider);
	Gui::Property("Light Radiance", light.Radiance, Gui::PropertyFlag::Color);
	Gui::Property("Light Multiplier", light.Multiplier, 0.0f, 5.0f, 0.25f, Gui::PropertyFlag::Slider);
	Gui::Property("Radiance Prefiltering", m_RadiancePrefilter);
	Gui::Property("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f, 0.5f, Gui::PropertyFlag::Slider);
	if ( Gui::Property("Mesh Bounding Boxes", m_UIShowMeshBoundingBoxes) )
	{
		ShowMeshBoundingBoxes(m_UIShowMeshBoundingBoxes);
	}
	if ( Gui::Property("Physics Bounding Boxes", m_UIShowPhysicsBodyBoundingBoxes) )
	{
		ShowPhysicsBodyBoundingBoxes(m_UIShowPhysicsBodyBoundingBoxes);
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
	ScriptEngine::SetSceneContext(GetShared());

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
