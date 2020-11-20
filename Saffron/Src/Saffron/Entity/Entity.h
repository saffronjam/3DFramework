#pragma once

#include <entt/entt.hpp>

#include "Saffron/Core/UUID.h"
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
	Entity(EntityHandle handle, Scene *scene);

	template<typename T, typename... Args>
	T &AddComponent(Args&&... args);
	template<typename T>
	T &GetComponent();
	template<typename T>
	const T &GetComponent() const;
	template<typename T>
	bool HasComponent() const;
	template<typename T>
	void RemoveComponent();

	Matrix4f &Transform() { return GetComponent<TransformComponent>(); }
	const Matrix4f &Transform() const { return GetComponent<TransformComponent>(); }

	operator Uint32 () const { return static_cast<Uint32>(m_Handle); }
	operator entt::entity() const { return m_Handle; }
	operator bool() const { return static_cast<Uint32>(m_Handle) && m_Scene; }
	bool operator==(const Entity &other) const;
	bool operator!=(const Entity &other) const;
	bool operator<(const Entity &other) const;

	UUID GetUUID() { return GetComponent<IDComponent>().ID; }
	UUID GetSceneUUID() const;
	Scene *GetScene() { return m_Scene; }
	const Scene *GetScene() const { return m_Scene; }
	EntityHandle GetHandle() const { return m_Handle; }

	Entity Copy(Optional<Scene *> separateScene = {});
	static Entity Null() { return { entt::null, nullptr }; }

private:
	class Scene *m_Scene = nullptr;
	EntityRegistry *m_Registry = nullptr;
	EntityHandle m_Handle{ entt::null };

	friend class SceneSerializer;
};

template <typename T, typename ... Args>
T &Entity::AddComponent(Args&&... args)
{
	SE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
	return m_Registry->emplace<T>(m_Handle, std::forward<Args>(args)...);
}

template <typename T>
T &Entity::GetComponent()
{
	SE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
	return m_Registry->get<T>(m_Handle);
}

template <typename T>
const T &Entity::GetComponent() const
{
	SE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
	return m_Registry->get<T>(m_Handle);
}

template <typename T>
bool Entity::HasComponent() const
{
	return m_Registry->has<T>(m_Handle);
}

template <typename T>
void Entity::RemoveComponent()
{
	SE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
	m_Registry->remove<T>(m_Handle);
}
}

