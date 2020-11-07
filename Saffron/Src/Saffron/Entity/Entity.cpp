#include "SaffronPCH.h"

#include "Saffron/Entity/Entity.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{

//////////////////////////////////////////////////////////////////////
/// Helper functions
//////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////
/// Entity
//////////////////////////////////////////////////////////////////////

Entity::Entity(EntityHandle handle, Scene *scene)
	: m_Scene(scene),
	m_Registry(&scene->GetEntityRegistry()),
	m_Handle(handle)
{
}

bool Entity::operator==(const Entity &other) const
{
	return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
}

bool Entity::operator!=(const Entity &other) const
{
	return !(*this == other);
}

bool Entity::operator<(const Entity &other) const
{
	return this->m_Handle < other.m_Handle;
}

UUID Entity::GetSceneUUID() const
{
	return m_Scene->GetUUID();
}

Entity Entity::Copy(Optional<Scene *> separateScene)
{
	Scene *copyTo = separateScene.value_or(m_Scene);

	Entity newEntity = copyTo->CreateEntity(GetComponent<TagComponent>().Tag);
	CopyComponent<MeshComponent>(newEntity, *this);
	CopyComponent<ScriptComponent>(newEntity, *this);
	CopyComponent<CameraComponent>(newEntity, *this);
	CopyComponent<SpriteRendererComponent>(newEntity, *this);
	CopyComponent<RigidBody2DComponent>(newEntity, *this);
	CopyComponent<BoxCollider2DComponent>(newEntity, *this);
	CopyComponent<CircleCollider2DComponent>(newEntity, *this);
	CopyComponent<RigidBody3DComponent>(newEntity, *this);
	CopyComponent<BoxCollider3DComponent>(newEntity, *this);
	CopyComponent<SphereCollider3DComponent>(newEntity, *this);

	return newEntity;
}
}
