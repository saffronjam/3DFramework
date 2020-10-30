#include "SaffronPCH.h"

#include "Saffron/Scene/ModelSpaceScene.h"


namespace Se
{

template<typename T>
static void CopyComponent(Entity destination, Entity source)
{
	if ( source.HasComponent<T>() )
	{
		auto &srcComponent = source.GetComponent<T>();
		destination.AddComponent<T>(srcComponent);
	}
}


ModelSpaceScene::ModelSpaceScene(Entity entity)
	: Scene(entity.GetComponent<TagComponent>().Tag),
	m_ViewportPane(entity.GetComponent<TagComponent>().Tag, SceneRenderer::GetMainTarget()),
	m_SavedTransform(entity.GetComponent<TransformComponent>().Transform)
{
	m_SceneEntity.AddComponent<EditorCameraComponent>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f));
	m_EditorCamera = m_SceneEntity.GetComponent<EditorCameraComponent>().Camera;

	m_Entity = CreateEntity(entity.GetComponent<IDComponent>().ID, entity.GetComponent<TagComponent>().Tag);
	m_Entity.AddComponent<TransformComponent>();

	CopyComponent<MeshComponent>(m_Entity, entity);
	CopyComponent<ScriptComponent>(m_Entity, entity);
	CopyComponent<CameraComponent>(m_Entity, entity);
	CopyComponent<SpriteRendererComponent>(m_Entity, entity);
	CopyComponent<RigidBody2DComponent>(m_Entity, entity);
	CopyComponent<BoxCollider2DComponent>(m_Entity, entity);
	CopyComponent<CircleCollider2DComponent>(m_Entity, entity);
}

void ModelSpaceScene::OnUpdate()
{
	m_SceneEntity.GetComponent<EditorCameraComponent>().Camera->OnUpdate();
}

void ModelSpaceScene::OnRender()
{
	m_Skybox.Material->Set("u_TextureLod", m_SkyboxLod);

	SceneRenderer::GetMainTarget()->SetCameraData({ Shared<Camera>::Cast(m_EditorCamera), m_EditorCamera->GetViewMatrix() });
	SceneRenderer::BeginScene(this);
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
}
}
