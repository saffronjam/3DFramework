#include "SaffronPCH.h"

#include "Saffron/Entity/Entity.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
//////////////////////////////////////////////////////////////////////
/// Helper functions
//////////////////////////////////////////////////////////////////////

template <typename T>
static void CopyComponent(Entity destination, Entity source)
{
	if (source.HasComponent<T>())
	{
		auto& srcComponent = source.GetComponent<T>();
		if (destination.HasComponent<T>())
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

Entity::Entity(EntityHandle handle, Scene* scene) :
	_scene(scene),
	_registry(&scene->GetEntityRegistry()),
	_handle(handle)
{
}

bool Entity::operator==(const Entity& other) const
{
	return _handle == other._handle && _scene == other._scene;
}

bool Entity::operator!=(const Entity& other) const
{
	return !(*this == other);
}

bool Entity::operator<(const Entity& other) const
{
	return this->_handle < other._handle;
}

UUID Entity::GetSceneUUID() const
{
	return _scene->GetUUID();
}

Entity Entity::Copy(Optional<Scene*> separateScene)
{
	Scene* copyTo = separateScene.value_or(_scene);

	Entity newEntity = copyTo->CreateEntity(GetComponent<TagComponent>().Tag);
	CopyComponent<TransformComponent>(newEntity, *this);
	CopyComponent<MeshComponent>(newEntity, *this);
	CopyComponent<CameraComponent>(newEntity, *this);
	CopyComponent<DirectionalLightComponent>(newEntity, *this);
	CopyComponent<SkylightComponent>(newEntity, *this);
	CopyComponent<ScriptComponent>(newEntity, *this);
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
