#include "SaffronPCH.h"

#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/collision/shapes/BoxShape.h>

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Entity/EntityComponents.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/EditorScene.h"
#include "Saffron/Serialize/SceneSerializer.h"

namespace Se
{
EditorScene::EditorScene(Filepath filepath)
	:
	Scene(),
	//m_MiniTarget(SceneRenderer::Target::Create("Mini target", 100, 100)),
	m_Filepath(Move(filepath))
{
	m_EditorCamera = m_SceneEntity.AddComponent<EditorCameraComponent>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)).Camera;
	//m_MiniTarget->Disable();

	if ( IsValidFilepath(m_Filepath) )
	{
		SceneSerializer serializer(*this);
		if ( !serializer.Deserialize(m_Filepath) )
		{
			SE_WARN("Failed to load scene! Filepath: {0}", m_Filepath.string());
			return;
		}
	}
}

void EditorScene::OnUpdate()
{
	m_EditorCamera->OnUpdate();
	if ( m_SelectedEntity )
	{
		if ( m_SelectedEntity.HasComponent<CameraComponent>() && m_SelectedEntity.HasComponent<TransformComponent>() )
		{
			std::shared_ptr<SceneCamera> camera = m_SelectedEntity.GetComponent<CameraComponent>().Camera;

			camera->SetViewportSize(static_cast<Uint32>(m_ViewportWidth), static_cast<Uint32>(m_ViewportHeight));
			const glm::mat4 cameraViewMatrix = glm::inverse(m_SelectedEntity.GetComponent<TransformComponent>().Transform);

			//m_MiniTarget->SetCameraData({ camera.get(), cameraViewMatrix });
			//m_MiniTarget->Enable();
		}
	}
}

void EditorScene::OnRender()
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

	m_Skybox.Material->Set("u_TextureLod", m_Skybox.Lod);

	SceneRenderer::GetMainTarget()->SetCameraData({ std::dynamic_pointer_cast<Camera>(m_EditorCamera), m_EditorCamera->GetViewMatrix(), 0.1f, 1000.0f, 45.0f });

	ArrayList<std::shared_ptr<SceneRenderer::Target>> targets = { SceneRenderer::GetMainTarget() };
	//if ( m_SceneEntity && m_SelectedEntity.HasComponent<CameraComponent>() )
	//{
	//	//targets.push_back(m_MiniTarget);
	//}

	SceneRenderer::BeginScene(this, { targets });
	auto meshGroup = m_EntityRegistry.group<MeshComponent>(entt::get<TransformComponent>);
	for ( const auto &entityHandle : meshGroup )
	{
		auto [meshComponent, transformComponent] = meshGroup.get<MeshComponent, TransformComponent>(entityHandle);
		if ( meshComponent.Mesh )
		{
			meshComponent.Mesh->OnUpdate();

			// TODO: Should we render (logically)
			m_SelectedEntity == entityHandle ? SceneRenderer::SubmitSelectedMesh(meshComponent, transformComponent) : SceneRenderer::SubmitMesh(meshComponent, transformComponent);
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

	SceneRenderer::EndScene();

	return;

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

}

void EditorScene::OnGuiRender()
{
	ImGui::Begin("Scene");

	Gui::BeginPropertyGrid();
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

	m_EditorCamera->OnGuiRender();
}

void EditorScene::OnEvent(const Event &event)
{
	m_EditorCamera->OnEvent(event);
}

void EditorScene::SetSelectedEntity(Entity entity)
{
	//m_MiniTarget->Disable();
	Scene::SetSelectedEntity(entity);
}

void EditorScene::SetViewportSize(Uint32 width, Uint32 height)
{
	m_EditorCamera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), static_cast<float>(width), static_cast<float>(height), 0.1f, 10000.0f));
	m_EditorCamera->SetViewportSize(width, height);
	Scene::SetViewportSize(width, height);
}

void EditorScene::Save() const
{
	if ( !m_Filepath.empty() && m_Filepath.extension() == ".ssc" )
	{
		auto &thisNonConst = const_cast<EditorScene &>(*this);
		SceneSerializer serializer(thisNonConst);
		serializer.Serialize(m_Filepath);
	}
}
}
