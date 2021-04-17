#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Entity/EntityComponents.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/EditorScene.h"
#include "Saffron/Serialize/SceneSerializer.h"

namespace Se
{
EditorScene::EditorScene(Filepath filepath) :
	m_Filepath(Move(filepath))
{
	m_EditorCamera = m_SceneEntity.AddComponent<EditorCameraComponent>(
		glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)).Camera;

	if (IsValidFilepath(m_Filepath))
	{
		SceneSerializer serializer(*this);
		if (!serializer.Deserialize(m_Filepath))
		{
			SE_WARN("Failed to load scene! Filepath: {0}", m_Filepath.string());
			return;
		}
	}
}

void EditorScene::OnUpdate()
{
	m_EditorCamera->OnUpdate();
	if (m_SelectedEntity)
	{
		if (m_SelectedEntity.HasComponent<CameraComponent>() && m_SelectedEntity.HasComponent<TransformComponent>())
		{
			Shared<SceneCamera> camera = m_SelectedEntity.GetComponent<CameraComponent>().Camera;

			camera->SetViewportSize(static_cast<Uint32>(m_ViewportWidth), static_cast<Uint32>(m_ViewportHeight));
			const glm::mat4 cameraViewMatrix = inverse(m_SelectedEntity.GetComponent<TransformComponent>().Transform);

			//m_MiniTarget->SetCameraData({ camera.get(), cameraViewMatrix });
			//m_MiniTarget->Enable();
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
		m_LightEnvironment = LightEnvironment();
		auto lights = m_EntityRegistry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
		uint32_t directionalLightIndex = 0;
		for (auto entity : lights)
		{
			auto [transformComponent, lightComponent] = lights.get<
				TransformComponent, DirectionalLightComponent>(entity);
			glm::vec3 direction = -normalize(glm::mat3(transformComponent.Transform) * glm::vec3(1.0f));
			m_LightEnvironment.DirectionalLights[directionalLightIndex++] = {
				direction, lightComponent.Radiance, lightComponent.Intensity, lightComponent.CastShadows
			};
		}
	}

	// TODO: only one sky light at the moment!
	{
		auto lights = m_EntityRegistry.group<SkylightComponent>(entt::get<TransformComponent>);
		for (auto entity : lights)
		{
			auto [transformComponent, skyLightComponent] = lights.get<TransformComponent, SkylightComponent>(entity);
			m_SceneEnvironment = skyLightComponent.SceneEnvironment;
			m_SceneEnvironment->SetIntensity(skyLightComponent.Intensity);
			SetSkybox(m_SceneEnvironment->GetRadianceMap());
		}
	}

	m_Skybox.Material->Set("u_TextureLod", m_Skybox.Lod);

	auto group = m_EntityRegistry.group<MeshComponent>(entt::get<TransformComponent>);
	SceneRenderer::BeginScene(this, {*m_EditorCamera, m_EditorCamera->GetViewMatrix(), 0.1f, 1000.0f, 45.0f});
	// TODO: real values
	for (auto entity : group)
	{
		auto& [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
		if (meshComponent.Mesh)
		{
			meshComponent.Mesh->OnUpdate();

			// TODO: Should we render (logically)

			if (m_SelectedEntity == entity) SceneRenderer::SubmitSelectedMesh(meshComponent, transformComponent);
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
	Gui::Property("Skybox LOD", m_Skybox.Lod, 0.0f, 11.0f, 0.5f, Gui::PropertyFlag::Drag);
	auto& light = GetLight();
	Gui::Property("Light Direction", light.Direction, Gui::PropertyFlag::Slider);
	Gui::Property("Light Radiance", light.Radiance, Gui::PropertyFlag::Color);
	Gui::Property("Light Multiplier", light.Multiplier, 0.0f, 5.0f, 0.25f, Gui::PropertyFlag::Slider);
	Gui::Property("Radiance Prefiltering", m_RadiancePrefilter);
	Gui::Property("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f, 0.5f, Gui::PropertyFlag::Slider);
	if (Gui::Property("Mesh Bounding Boxes", m_UIShowMeshBoundingBoxes))
	{
		ShowMeshBoundingBoxes(m_UIShowMeshBoundingBoxes);
	}
	if (Gui::Property("Physics Bounding Boxes", m_UIShowPhysicsBodyBoundingBoxes))
	{
		ShowPhysicsBodyBoundingBoxes(m_UIShowPhysicsBodyBoundingBoxes);
	}
	Gui::EndPropertyGrid();

	ImGui::End();

	if (m_SceneEntity.HasComponent<PhysicsWorld2DComponent>())
	{
		m_SceneEntity.GetComponent<PhysicsWorld2DComponent>().World.OnGuiRender();
	}
	if (m_SceneEntity.HasComponent<PhysicsWorld3DComponent>())
	{
		m_SceneEntity.GetComponent<PhysicsWorld3DComponent>().World.OnGuiRender();
	}

	m_EditorCamera->OnGuiRender();
}

void EditorScene::SetSelectedEntity(Entity entity)
{
	//m_MiniTarget->Disable();
	Scene::SetSelectedEntity(entity);
}

void EditorScene::SetViewportSize(Uint32 width, Uint32 height)
{
	m_EditorCamera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), static_cast<float>(width),
	                                                        static_cast<float>(height), 0.1f, 10000.0f));
	m_EditorCamera->SetViewportSize(width, height);
	Scene::SetViewportSize(width, height);
}

void EditorScene::Save() const
{
	if (!m_Filepath.empty() && m_Filepath.extension() == ".ssc")
	{
		auto& thisNonConst = const_cast<EditorScene&>(*this);
		SceneSerializer serializer(thisNonConst);
		serializer.Serialize(m_Filepath);
	}
}
}
