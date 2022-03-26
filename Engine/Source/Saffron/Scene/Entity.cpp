#include "SaffronPCH.h"

#include "Saffron/Scene/Entity.h"

namespace Se
{
auto Entity::Id() const -> Uuid
{
	return Get<IdComponent>().Id;
}

auto Entity::Valid() -> bool
{
	return _scene != nullptr && _scene->Registry().valid(ToNativeHandle(_id));
}

auto Entity::Null() -> Entity
{
	return Entity{ Uuid::Null(), nullptr };
}

entt::entity Entity::ToNativeHandle(Uuid id)
{
	return static_cast<entt::entity>(static_cast<ulong>(id));
}
}
