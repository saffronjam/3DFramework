#include "SaffronPCH.h"

#include "Saffron/Core/Misc.h"
#include "Saffron/Core/Global.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/RuntimeScene.h"
#include "Saffron/Script/ScriptEngine.h"

namespace Se
{
template <typename T>
static void CopyComponent(EntityRegistry& dstRegistry, EntityRegistry& srcRegistry, const TreeMap<UUID, Entity>& entityMap)
{
	auto components = srcRegistry.view<T>();
	for (auto entityHandle : components)
	{
		Entity destEntity = entityMap.at(srcRegistry.get<IDComponent>(entityHandle).ID);

		auto& srcComponent = srcRegistry.get<T>(entityHandle);
		auto& destComponent = dstRegistry.emplace_or_replace<T>(destEntity.GetHandle(), srcComponent);
	}
}

RuntimeScene::RuntimeScene(Shared<Scene> copyFrom)
{
	Debug::Assert(copyFrom, "Runtime Scene must have a scene to copy from");;

	// Environment
	_light = copyFrom->GetLight();
	_lightMultiplier = copyFrom->GetLight().Multiplier;

	_sceneEnvironment = copyFrom->GetSceneEnvironment();
	_skybox = copyFrom->GetSkybox();

	auto& otherRegistry = copyFrom->GetEntityRegistry();
	TreeMap<UUID, Entity> entityMap;
	auto idComponents = otherRegistry.view<IDComponent>();
	for (auto entityHandle : idComponents)
	{
		auto uuid = otherRegistry.get<IDComponent>(entityHandle).ID;
		auto tag = otherRegistry.get<TagComponent>(entityHandle).Tag;
		const Entity entity = CreateEntity(uuid, tag);
		entityMap[uuid] = entity;
	}

	CopyComponent<TagComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<TransformComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<MeshComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<CameraComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<DirectionalLightComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<SkylightComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<ScriptComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<SpriteRendererComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<RigidBody2DComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<BoxCollider2DComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<CircleCollider2DComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<RigidBody3DComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<BoxCollider3DComponent>(_entityRegistry, otherRegistry, entityMap);
	CopyComponent<SphereCollider3DComponent>(_entityRegistry, otherRegistry, entityMap);

	const auto& entityInstanceMap = ScriptEngine::GetEntityInstanceMap();
	if (entityInstanceMap.find(GetUUID()) != entityInstanceMap.end())
	{
		ScriptEngine::CopyEntityScriptData(GetUUID(), copyFrom->GetUUID());
	}

	if (copyFrom->GetEntity().HasComponent<PhysicsWorld2DComponent>())
	{
		auto& copyFromWorld = copyFrom->GetEntity().GetComponent<PhysicsWorld2DComponent>().World;
		_sceneEntity.GetComponent<PhysicsWorld2DComponent>().World.SetGravity(copyFromWorld.GetGravity());
	}
	if (copyFrom->GetEntity().HasComponent<PhysicsWorld3DComponent>())
	{
		auto& copyFromWorld = copyFrom->GetEntity().GetComponent<PhysicsWorld3DComponent>().World;
		//_sceneEntity.GetComponent<PhysicsWorld3DComponent>().World.SetGravity(copyFromWorld.GetGravity());
	}

	_debugCamera = _sceneEntity.AddComponent<EditorCameraComponent>(
		glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)).Camera;
}

void RuntimeScene::OnUpdate()
{
	const auto ts = Global::Timer::GetStep();

	auto view = _entityRegistry.view<ScriptComponent>();
	for (auto entityHandle : view)
	{
		const UUID entityID = _entityRegistry.get<IDComponent>(entityHandle).ID;
		Entity entity = {entityHandle, this};
		if (ScriptEngine::ModuleExists(entity.GetComponent<ScriptComponent>().ModuleName))
		{
			ScriptEngine::OnUpdateEntity(_sceneID, entityID, ts);
		}
	}


	if (_debugCameraActivated && Keyboard::IsPressed(KeyCode::LControl))
	{
		_debugCamera->OnUpdate();
	}

	if (_sceneEntity.HasComponent<PhysicsWorld2DComponent>())
	{
		_sceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnUpdate();
	}
	if (_sceneEntity.HasComponent<PhysicsWorld3DComponent>())
	{
		//_sceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnUpdate();
	}
}

void RuntimeScene::OnRender()
{
	/////////////////////////////////////////////////////////////////////
	// RENDER 3D SCENE
	/////////////////////////////////////////////////////////////////////
	Entity cameraEntity = GetMainCameraEntity();
	if (!cameraEntity) return;

	// Process camera entity
	glm::mat4 cameraViewMatrix = inverse(cameraEntity.GetComponent<TransformComponent>().Transform);
	Debug::Assert(cameraEntity, "Scene does not contain any cameras!");;
	SceneCamera& camera = *cameraEntity.GetComponent<CameraComponent>().Camera;
	camera.SetViewportSize(_viewportWidth, _viewportHeight);

	// Process lights
	{
		_lightEnvironment = LightEnvironment();
		auto lights = _entityRegistry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
		uint32_t directionalLightIndex = 0;
		for (auto entity : lights)
		{
			auto [transformComponent, lightComponent] = lights.get<
				TransformComponent, DirectionalLightComponent>(entity);
			glm::vec3 direction = -normalize(glm::mat3(transformComponent.Transform) * glm::vec3(1.0f));
			_lightEnvironment.DirectionalLights[directionalLightIndex++] = {
				direction, lightComponent.Radiance, lightComponent.Intensity, lightComponent.CastShadows
			};
		}
	}

	// TODO: only one sky light at the moment!
	{
		auto lights = _entityRegistry.group<SkylightComponent>(entt::get<TransformComponent>);
		for (auto entity : lights)
		{
			auto [transformComponent, skyLightComponent] = lights.get<TransformComponent, SkylightComponent>(entity);
			_sceneEnvironment = skyLightComponent.SceneEnvironment;
			_sceneEnvironment->SetIntensity(skyLightComponent.Intensity);
			SetSkybox(_sceneEnvironment->GetRadianceMap());
		}
	}

	_skybox.Material->Set("u_TextureLod", _skybox.Lod);

	auto group = _entityRegistry.group<MeshComponent>(entt::get<TransformComponent>);
	SceneRenderer::BeginScene(this, {camera, cameraViewMatrix});
	for (auto entity : group)
	{
		auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
		if (meshComponent.Mesh)
		{
			meshComponent.Mesh->OnUpdate();

			// TODO: Should we render (logically)
			SceneRenderer::SubmitMesh(meshComponent, transformComponent, nullptr);
		}
	}
	SceneRenderer::EndScene();
	/////////////////////////////////////////////////////////////////////
}

void RuntimeScene::OnGuiRender()
{
	ImGui::Begin("Scene");

	Gui::BeginPropertyGrid();
	Gui::Property("Debug Camera", _debugCameraActivated);
	Gui::Property("Skybox LOD", _skybox.Lod, 0.0f, 11.0f, 0.5f, Gui::PropertyFlag::Drag);
	auto& light = GetLight();
	Gui::Property("Light Direction", light.Direction, Gui::PropertyFlag::Slider);
	Gui::Property("Light Radiance", light.Radiance, Gui::PropertyFlag::Color);
	Gui::Property("Light Multiplier", light.Multiplier, 0.0f, 5.0f, 0.25f, Gui::PropertyFlag::Slider);
	Gui::Property("Radiance Prefiltering", _radiancePrefilter);
	Gui::Property("Env Map Rotation", _envMapRotation, -360.0f, 360.0f, 0.5f, Gui::PropertyFlag::Slider);
	if (Gui::Property("Mesh Bounding Boxes", _uIShowMeshBoundingBoxes))
	{
		ShowMeshBoundingBoxes(_uIShowMeshBoundingBoxes);
	}
	if (Gui::Property("Physics Bounding Boxes", _uIShowPhysicsBodyBoundingBoxes))
	{
		ShowPhysicsBodyBoundingBoxes(_uIShowPhysicsBodyBoundingBoxes);
	}
	Gui::EndPropertyGrid();

	ImGui::End();

	if (_sceneEntity.HasComponent<PhysicsWorld2DComponent>())
	{
		_sceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnGuiRender();
	}
	if (_sceneEntity.HasComponent<PhysicsWorld3DComponent>())
	{
		//_sceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnGuiRender();
	}

	_debugCamera->OnGuiRender();
}

void RuntimeScene::OnStart()
{
	ScriptEngine::SetSceneContext(this);

	{
		auto view = _entityRegistry.view<ScriptComponent>();
		for (auto eent : view)
		{
			Entity entity = {eent, this};
			auto component = entity.GetComponent<ScriptComponent>();
			if (ScriptEngine::ModuleExists(component.ModuleName))
			{
				ScriptEngine::InstantiateEntityClass(entity);
			}
		}
	}

	if (_sceneEntity.HasComponent<PhysicsWorld2DComponent>())
	{
		_sceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnStart();
	}
	if (_sceneEntity.HasComponent<PhysicsWorld3DComponent>())
	{
		//_sceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnStart();
	}
}

void RuntimeScene::OnStop()
{
	if (_sceneEntity.HasComponent<PhysicsWorld2DComponent>())
	{
		_sceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnStop();
	}
	if (_sceneEntity.HasComponent<PhysicsWorld3DComponent>())
	{
		//_sceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnStop();
	}
}

void RuntimeScene::Escape()
{
	_debugCameraActivated = true;
	_debugCamera->Enable();
}

void RuntimeScene::Return()
{
	_debugCameraActivated = false;
	_debugCamera->Disable();
}
}
