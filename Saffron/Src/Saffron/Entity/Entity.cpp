#include "SaffronPCH.h"

#include "Saffron/Entity/Entity.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
Entity::Entity(EntityHandle handle, Scene *scene)
	: m_Scene(scene),
	m_Registry(&scene->GetEntityRegistry()),
	m_EntityHandle(handle)
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

bool Entity::operator<(const Entity &other) const
{
	return this->m_EntityHandle < other.m_EntityHandle;
}

UUID Entity::GetSceneUUID() const
{
	return m_Scene->GetUUID();
}

}
