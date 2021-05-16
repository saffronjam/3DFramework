#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Memory.h"
#include "Saffron/Rendering/Resource.h"

namespace Se
{
class ResourceManager : public SingleTon<ResourceManager>
{
public:
	ResourceManager();

	static const Shared<Resource>& Get(const Shared<Resource>& resource);
	static const Shared<Resource>& Get(ulong identifier);
	static const Shared<Resource>& GetFallback(const String& identifier);

	static bool Contains(const Shared<Resource>& resource);
	static bool Contains(ulong identifier);
	static bool ContainsFallback(const String& identifier);

	static void Add(Shared<Resource> resource);
	static void Add(Shared<Resource> resource, ulong identifier);
	static void AddFallback(Shared<Resource> resource, String identifier);

	static const List<Shared<Resource>>& GetAll();
	static void Clear();

	template <class ResourceType>
	static void ForEachResource(Function<void(ResourceType&)> function);
	template <class ResourceType>
	static void ForEachResource(Function<void(ResourceType&)> function, Function<bool(ResourceType&)> condition);

private:
	void SyncCache();

private:
	HashMap<ulong, Shared<Resource>> _resources;
	HashMap<String, Shared<Resource>> _fallbacks;
	
	List<Shared<Resource>> _resourceReturnCache;
	bool _needCacheSync = true;
};

template <class ResourceType>
void ResourceManager::ForEachResource(Function<void(ResourceType&)> function)
{
	static_assert(std::is_base_of<Resource, ResourceType>::value, "ResourceType was not base of Resource");
	
	auto& instance = Instance();
	Shared<ResourceType> dyncastResource = nullptr;

	for (auto iter = instance._resources.begin(); iter != instance._resources.end(); ++iter)
	{
		if ((dyncastResource = dynamic_cast<ResourceType*>(iter->second.Raw())))
		{
			function(*dyncastResource);
		}
	}
}

template <class ResourceType>
void ResourceManager::ForEachResource(Function<void(ResourceType&)> function, Function<bool(ResourceType&)> condition)
{
	static_assert(std::is_base_of<ResourceType, Resource>::value, "ResourceType was not base of Resource");
	
	auto &instance = Instance();
	ResourceType* dyncastResource = nullptr;

	for (auto iter = instance._resources.begin(); iter != instance._resources.end(); ++iter)
	{
		if ((dyncastResource = dynamic_cast<ResourceType*>(iter->second.Raw())))
		{
			if (condition(*dyncastResource))
			{
				function(*dyncastResource);
			}
		}
	}
}
}
