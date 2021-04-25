#pragma once

#include "Saffron/Core/Managed.h"
#include "Saffron/Core/Memory.h"
#include "Saffron/Rendering/Resource.h"

namespace Se
{
class ResourceManager : public Managed
{
public:
	static Shared<Resource>& Get(Shared<Resource> resource);
	static Shared<Resource>& Get(size_t identifer);
	static bool Exists(Shared<Resource> resource);
	static bool Exists(size_t identifier);
	static void Emplace(Shared<Resource> resource);
	static void Emplace(Shared<Resource> resource, size_t identifier);
	static const ArrayList<Shared<Resource>>& GetAll();
	static void Clear();

	template <class ResourceType>
	static void ForEach(Function<void(ResourceType&)> function);
	template <class ResourceType>
	static void ForEach(Function<void(ResourceType&)> function, Function<bool(ResourceType&)> condition);

private:
	static Shared<ResourceManager>& GetInstance();
	void SyncCache();

private:
	TreeMap<size_t, Shared<Resource>> _memory;
	ArrayList<Shared<Resource>> _returnCache;
	bool _needCacheSync = true;
};

template <class ResourceType>
void ResourceManager::ForEach(Function<void(ResourceType&)> function)
{
	auto instance = GetInstance();
	ResourceType* dyncastResource = nullptr;

	for (auto iter = instance->_memory.begin(); iter != instance->_memory.end(); ++iter)
	{
		if ((dyncastResource = dynamic_cast<ResourceType*>(iter->second.Raw())))
		{
			function(*dyncastResource);
		}
	}
}

template <class ResourceType>
void ResourceManager::ForEach(Function<void(ResourceType&)> function, Function<bool(ResourceType&)> condition)
{
	auto instance = GetInstance();
	ResourceType* dyncastResource = nullptr;

	for (auto iter = instance->_memory.begin(); iter != instance->_memory.end(); ++iter)
	{
		if ((dyncastResource = dynamic_cast<ResourceType*>(iter->second.get())))
		{
			if (condition(*dyncastResource))
			{
				function(*dyncastResource);
			}
		}
	}
}
}
