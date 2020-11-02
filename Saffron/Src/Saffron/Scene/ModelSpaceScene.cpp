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
		if ( destination.HasComponent<T>() )
		{
			destination.GetComponent<T>() = srcComponent;
		}
		else
		{
			destination.AddComponent<T>(srcComponent);
		}
	}
}

ModelSpaceScene::ModelSpaceScene(Entity entity)
	: Scene(entity.GetComponent<TagComponent>().Tag),
	m_Target(SceneRenderer::Target::Create(100, 100)),
	m_SavedTransform(entity.GetComponent<TransformComponent>().Transform)
{
	m_EditorCamera = m_SceneEntity.AddComponent<EditorCameraComponent>(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)).Camera;

	m_Entity = CreateEntity(entity.GetComponent<IDComponent>().ID, entity.GetComponent<TagComponent>().Tag);

	if ( entity.HasComponent<MeshComponent>() )
	{
		auto &otherMesh = entity.GetComponent<MeshComponent>().Mesh;
		m_Entity.AddComponent<MeshComponent>().Mesh = Shared<Mesh>::Create(otherMesh->GetFilepath());
		auto &mesh = m_Entity.GetComponent<MeshComponent>().Mesh;
		mesh->SetLocalTransform(otherMesh->GetLocalTransform());
	}
	CopyComponent<ScriptComponent>(m_Entity, entity);
	CopyComponent<CameraComponent>(m_Entity, entity);
	CopyComponent<SpriteRendererComponent>(m_Entity, entity);
	CopyComponent<RigidBody2DComponent>(m_Entity, entity);
	CopyComponent<BoxCollider2DComponent>(m_Entity, entity);
	CopyComponent<CircleCollider2DComponent>(m_Entity, entity);
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
