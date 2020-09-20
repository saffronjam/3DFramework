#include "SaffronPCH.h"

#include "Saffron/Scene/Entity.h"

namespace Se
{
Entity::Entity(entt::entity handle, Scene *scene) : m_EntityHandle(handle), m_Scene(scene)
{
}

bool Entity::operator==(const Entity &other) const
{
	return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
}

bool Entity::operator!=(const Entity &other) const
{
	return !(*this == other);
}

Entity::Entity(const std::string &name)
{
}
}
