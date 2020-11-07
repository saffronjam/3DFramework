#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/EditorScene.h"

namespace Se
{
EditorScene::EditorScene(String name)
	: Scene(Move(name)),
	m_MiniTarget(SceneRenderer::Target::Create(100, 100))
{
	m_EditorCamera = m_SceneEntity.AddComponent<EditorCameraComponent>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)).Camera;
	m_MiniTarget->Disable();
}

void EditorScene::OnUpdate()
{
	m_EditorCamera->OnUpdate();
	if ( m_SelectedEntity )
	{
		if ( m_SelectedEntity.HasComponent<CameraComponent>() && m_SelectedEntity.HasComponent<TransformComponent>() )
		{
			Shared<SceneCamera> camera = m_SelectedEntity.GetComponent<CameraComponent>().Camera;

			camera->SetViewportSize(static_cast<Uint32>(m_ViewportWidth), static_cast<Uint32>(m_ViewportHeight));
			const glm::mat4 cameraViewMatrix = glm::inverse(m_SelectedEntity.GetComponent<TransformComponent>().Transform);

			m_MiniTarget->SetCameraData({ camera.Raw(), cameraViewMatrix });
			m_MiniTarget->Enable();
		}
	}
}

void EditorScene::OnRender()
{
	m_Skybox.Material->Set("u_TextureLod", m_SkyboxLod);

	SceneRenderer::GetMainTarget()->SetCameraData({ Shared<Camera>::Cast(m_EditorCamera), m_EditorCamera->GetViewMatrix() });

	ArrayList<Shared<SceneRenderer::Target>> targets = { SceneRenderer::GetMainTarget() };
	if ( m_SceneEntity && m_SelectedEntity.HasComponent<CameraComponent>() )
	{
		targets.push_back(m_MiniTarget);
	}

	SceneRenderer::BeginScene(this, targets);
	auto group = m_EntityRegistry.group<MeshComponent>(entt::get<TransformComponent>);
	for ( auto entity : group )
	{
		auto [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
		if ( meshComponent.Mesh )
		{
			meshComponent.Mesh->OnUpdate();

			// TODO: Should we render (logically)

			if ( m_SelectedEntity == entity )
				SceneRenderer::SubmitSelectedMesh(meshComponent, transformComponent);
			else
				SceneRenderer::SubmitMesh(meshComponent, transformComponent);
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

void EditorScene::OnGuiRender()
{
	ImGui::Begin("Scene");

	Gui::BeginPropertyGrid();
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

	m_EditorCamera->OnGuiRender();
}

void EditorScene::OnEvent(const Event &event)
{
	m_EditorCamera->OnEvent(event);
}

void EditorScene::SetSelectedEntity(Entity entity)
{
	m_MiniTarget->Disable();
	Scene::SetSelectedEntity(entity);
}

void EditorScene::SetViewportSize(Uint32 width, Uint32 height)
{
	m_EditorCamera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), static_cast<float>(width), static_cast<float>(height), 0.1f, 10000.0f));
	m_EditorCamera->SetViewportSize(width, height);
	Scene::SetViewportSize(width, height);
}
}
