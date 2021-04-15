#pragma once

#include "Saffron/Base.h"
#include "Saffron/Resource/Resource.h"

namespace Se
{
class Factory
{
public:
	template<class t_Resource, class... t_Args >
	static Shared<t_Resource> Create(t_Args &&... args);
};

template <class t_Resource, class... t_Args >
Shared<t_Resource> Factory::Create(t_Args &&... args)
{
	static_assert(std::is_base_of<Resource, t_Resource>::value, "Factory::Create can only create resources");

	Shared<t_Resource> resource;

	resource = t_Resource::Create(std::forward<t_Args>(args)...);
	resource->Initialize();

	return resource;
}
}
