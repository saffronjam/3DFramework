#include "SaffronPCH.h"

#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/ModelSpaceScene.h"


namespace Se
{
ModelSpaceScene::ModelSpaceScene(Entity entity)
	:
	m_Target(SceneRenderer::Target::Create(100, 100)),
	m_SavedTransform(entity.GetComponent<TransformComponent>().Transform)
{
	m_EditorCamera = m_SceneEntity.AddComponent<EditorCameraComponent>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)).Camera;

	m_Entity = entity.Copy(this);
	m_Entity.GetComponent<TransformComponent>().Transform = Matrix4f(1);

	ModelSpaceScene::SetSelectedEntity(m_Entity);
}

void ModelSpaceScene::OnUpdate()
{
	m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->OnUpdate();
}

void ModelSpaceScene::OnRender()
{
	m_Skybox.Material->Set("u_TextureLod", m_SkyboxLod);

	m_Target->SetCameraData({ Shared<Camera>::Cast(m_EditorCamera), m_EditorCamera->GetViewMatrix() });
	SceneRenderer::BeginScene(this, { m_Target });
	if ( m_Entity.HasComponent<MeshComponent>() )
	{
		auto [meshComponent, transformComponent] = m_EntityRegistry.get<MeshComponent, TransformComponent>(m_Entity);
		if ( meshComponent.Mesh )
		{
			meshComponent.Mesh->OnUpdate();

			SceneRenderer::SubmitMesh(meshComponent, transformComponent);
		}
	}
	SceneRenderer::EndScene();
}

void ModelSpaceScene::OnGuiRender()
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

	m_EditorCamera->OnGuiRender();
}

void ModelSpaceScene::SetViewportSize(Uint32 width, Uint32 height)
{
	m_EditorCamera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), static_cast<float>(width), static_cast<float>(height), 0.1f, 10000.0f));
	m_EditorCamera->SetViewportSize(width, height);
	Scene::SetViewportSize(width, height);
}

void ModelSpaceScene::SetSelectedEntity(Entity entity)
{
	SE_CORE_ASSERT(entity == m_Entity, "Can't change selection context in a Model Space Scene");
	Scene::SetSelectedEntity(entity);
}
}
