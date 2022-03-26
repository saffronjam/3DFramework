#pragma once

#include "entt/entt.hpp"

#include "Saffron/Base.h"
#include "Saffron/Scene/Scene.h"

namespace Se
{
class Entity
{
public:
	Entity() :
		_id(Uuid::Null()),
		_scene(nullptr)
	{
	}

	Entity(Uuid id, Scene* scene) :
		_id(id),
		_scene(scene)
	{
	}

	auto Id() const -> Uuid;


	template <typename ComponentType, typename ... Args>
	auto Add(Args&& ... args) -> ComponentType&;

	template <typename ComponentType>
	auto Remove() -> bool;

	template <typename ComponentType>
	auto Get() -> ComponentType&;

	template <typename ComponentType>
	auto Get() const -> ComponentType&;

	template <typename ComponentType>
	auto Has() const -> bool;

	auto Valid() -> bool;

	static auto Null() -> Entity;

	static auto ToNativeHandle(Uuid id) -> entt::entity;

private:
	Uuid _id;
	Scene* _scene;
};

template <typename ComponentType, typename ... Args>
auto Entity::Add(Args&&... args) -> ComponentType&
{
	return _scene->Registry().emplace<ComponentType>(
		static_cast<entt::entity>(static_cast<const ulong>(_id)),
		std::forward<Args>(args)...
	);
}

template <typename ComponentType>
auto Entity::Remove() -> bool
{
	return _scene->Registry().remove<ComponentType>(static_cast<entt::entity>(static_cast<const ulong>(_id))) > 0;
}

template <typename ComponentType>
auto Entity::Get() -> ComponentType&
{
	return _scene->Registry().get<ComponentType>(static_cast<entt::entity>(static_cast<const ulong>(_id)));
}

template <typename ComponentType>
auto Entity::Get() const -> ComponentType&
{
	return const_cast<Entity&>(*this).Get<ComponentType>();
}

template <typename ComponentType>
auto Entity::Has() const -> bool
{
	return _scene->Registry().any_of<ComponentType>(static_cast<entt::entity>(static_cast<const ulong>(_id)));
}
}
