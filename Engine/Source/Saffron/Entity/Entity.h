#pragma once

#include <entt/entt.hpp>

#include "Saffron/Base.h"
#include "Saffron/Entity/EntityHandle.h"
#include "Saffron/Entity/EntityRegistry.h"
#include "Saffron/Entity/EntityComponents.h"

namespace Se
{
class Scene;

class Entity
{
public:
	Entity() = default;
	Entity(EntityHandle handle, Scene* scene);

	template <typename T, typename... Args>
	T& AddComponent(Args&&... args);
	template <typename T>
	T& GetComponent();
	template <typename T>
	const T& GetComponent() const;
	template <typename T>
	bool HasComponent() const;
	template <typename T>
	void RemoveComponent();

	Matrix4& Transform();
	const Matrix4& Transform() const;

	operator uint() const;
	operator EntityHandle() const;
	operator bool() const;

	bool operator==(const Entity& other) const;
	bool operator!=(const Entity& other) const;
	bool operator<(const Entity& other) const;

	UUID GetUUID();
	UUID GetSceneUUID() const;
	Scene* GetScene();
	const Scene* GetScene() const;
	EntityHandle GetHandle() const;

	Entity Copy(Optional<Scene*> separateScene = {});

	static Entity Null() { return {entt::null, nullptr}; }

private:
	class Scene* _scene = nullptr;
	EntityRegistry* _registry = nullptr;
	EntityHandle _handle{entt::null};

	friend class SceneSerializer;
};

template <typename T, typename ... Args>
T& Entity::AddComponent(Args&&... args)
{
	Debug::Assert(!HasComponent<T>(), "Entity already has component!");;
	return _registry->emplace<T>(_handle, std::forward<Args>(args)...);
}

template <typename T>
T& Entity::GetComponent()
{
	Debug::Assert(HasComponent<T>(), "Entity doesn't have component!");;
	return _registry->get<T>(_handle);
}

template <typename T>
const T& Entity::GetComponent() const
{
	Debug::Assert(HasComponent<T>(), "Entity doesn't have component!");;
	return _registry->get<T>(_handle);
}

template <typename T>
bool Entity::HasComponent() const
{
	return _registry->has<T>(_handle);
}

template <typename T>
void Entity::RemoveComponent()
{
	Debug::Assert(HasComponent<T>(), "Entity doesn't have component!");;
	_registry->remove<T>(_handle);
}
}
