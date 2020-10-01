#pragma once

#include <entt/entt.hpp>

#include "Saffron/Core/UUID.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/Components.h"

namespace Se
{
class Entity
{
public:
	Entity() = default;
	Entity(entt::entity handle, Scene *scene);
	~Entity() = default;

	template<typename T, typename... Args>
	T &AddComponent(Args&&... args);
	template<typename T>
	T &GetComponent();
	template<typename T>
	const T &GetComponent() const;
	template<typename T>
	bool HasComponent();
	template<typename T>
	void RemoveComponent();

	glm::mat4 &Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
	const glm::mat4 &Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }

	operator Uint32 () const { return static_cast<Uint32>(m_EntityHandle); }
	operator entt::entity() const { return m_EntityHandle; }
	operator bool() const { return static_cast<Uint32>(m_EntityHandle) && m_Scene; }
	bool operator==(const Entity &other) const;
	bool operator!=(const Entity &other) const;

	UUID GetUUID() { return GetComponent<IDComponent>().ID; }
	UUID GetSceneUUID() const { return m_Scene->GetUUID(); }

private:
	explicit Entity(const std::string &name);

private:
	entt::entity m_EntityHandle{ entt::null };
	Scene *m_Scene = nullptr;

	friend class Scene;
	friend class SceneSerializer;
	friend class ScriptEngine;
};

template <typename T, typename ... Args>
T &Entity::AddComponent(Args&&... args)
{
	SE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
	return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
}

template <typename T>
T &Entity::GetComponent()
{
	SE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
	return m_Scene->m_Registry.get<T>(m_EntityHandle);
}

template <typename T>
const T &Entity::GetComponent() const
{
	SE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
	return m_Scene->m_Registry.get<T>(m_EntityHandle);
}

template <typename T>
bool Entity::HasComponent()
{
	return m_Scene->m_Registry.has<T>(m_EntityHandle);
}

template <typename T>
void Entity::RemoveComponent()
{
	SE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
	m_Scene->m_Registry.remove<T>(m_EntityHandle);
}
}

