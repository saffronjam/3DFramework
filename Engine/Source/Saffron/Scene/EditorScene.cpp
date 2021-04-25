#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Entity/EntityComponents.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/EditorScene.h"
#include "Saffron/Serialize/SceneSerializer.h"

namespace Se
{
EditorScene::EditorScene(Filepath filepath) :
	_filepath(Move(filepath))
{
	_editorCamera = _sceneEntity.AddComponent<EditorCameraComponent>(
		glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)).Camera;

	if (IsValidFilepath(_filepath))
	{
		SceneSerializer serializer(*this);
		if (!serializer.Deserialize(_filepath))
		{
			Log::Warn("Failed to load scene! Filepath: {0}", _filepath.string());
			return;
		}
	}
}

void EditorScene::OnUpdate()
{
	_editorCamera->OnUpdate();
	if (_selectedEntity)
	{
		if (_selectedEntity.HasComponent<CameraComponent>() && _selectedEntity.HasComponent<TransformComponent>())
		{
			Shared<SceneCamera> camera = _selectedEntity.GetComponent<CameraComponent>().Camera;

			camera->SetViewportSize(static_cast<uint>(_viewportWidth), static_cast<uint>(_viewportHeight));
			const glm::mat4 cameraViewMatrix = inverse(_selectedEntity.GetComponent<TransformComponent>().Transform);

			//_miniTarget->SetCameraData({ camera.get(), cameraViewMatrix });
			//_miniTarget->Enable();
		}
	}
}

void EditorScene::OnRender()
{
	/////////////////////////////////////////////////////////////////////
	// RENDER 3D SCENE
	/////////////////////////////////////////////////////////////////////

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
	SceneRenderer::BeginScene(this, {*_editorCamera, _editorCamera->GetViewMatrix(), 0.1f, 1000.0f, 45.0f});
	// TODO: real values
	for (auto entity : group)
	{
		auto& [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
		if (meshComponent.Mesh)
		{
			meshComponent.Mesh->OnUpdate();

			// TODO: Should we render (logically)

			if (_selectedEntity == entity) SceneRenderer::SubmitSelectedMesh(meshComponent, transformComponent);
			else SceneRenderer::SubmitMesh(meshComponent, transformComponent);
		}
	}
	SceneRenderer::EndScene();
	/////////////////////////////////////////////////////////////////////
}

void EditorScene::OnGuiRender()
{
	ImGui::Begin("Scene");

	Gui::BeginPropertyGrid();
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

	_editorCamera->OnGuiRender();
}

void EditorScene::SetSelectedEntity(Entity entity)
{
	//_miniTarget->Disable();
	Scene::SetSelectedEntity(entity);
}

void EditorScene::SetViewportSize(uint width, uint height)
{
	_editorCamera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), static_cast<float>(width),
	                                                        static_cast<float>(height), 0.1f, 10000.0f));
	_editorCamera->SetViewportSize(width, height);
	Scene::SetViewportSize(width, height);
}

void EditorScene::Save() const
{
	if (!_filepath.empty() && _filepath.extension() == ".ssc")
	{
		auto& thisNonConst = const_cast<EditorScene&>(*this);
		SceneSerializer serializer(thisNonConst);
		serializer.Serialize(_filepath);
	}
}
}
