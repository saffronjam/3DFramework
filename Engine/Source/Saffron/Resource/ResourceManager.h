#pragma once

#include "Saffron/Core/MemManaged.h"
#include "Saffron/Resource/Resource.h"

namespace Se
{
class ResourceManager : public MemManaged<ResourceManager>
{
public:
	static Shared<Resource> &Get(Shared<Resource> resource);
	static Shared<Resource> &Get(size_t identifer);
	static bool Exists(Shared<Resource> resource);
	static bool Exists(size_t identifier);
	static void Emplace(Shared<Resource> resource);
	static void Emplace(Shared<Resource> resource, size_t identifier);
	static const ArrayList<Shared<Resource>> &GetAll();
	static void Clear();

	template<class ResourceType>
	static void ForEach(Function<void(ResourceType &)> function);
	template<class ResourceType>
	static void ForEach(Function<void(ResourceType &)> function, Function<bool(ResourceType &)> condition);

private:
	static Shared<ResourceManager> &GetInstance();
	void SyncCache();

private:
	Map<size_t, Shared<Resource>> m_Memory;
	ArrayList<Shared<Resource>> m_ReturnCache;
	bool m_NeedCacheSync = true;

};

template <class ResourceType>
void ResourceManager::ForEach(Function<void(ResourceType &)> function)
{
	auto instance = GetInstance();
	ResourceType *dyncastResource = nullptr;

	for ( auto iter = instance->m_Memory.begin(); iter != instance->m_Memory.end(); ++iter )
	{
		if ( (dyncastResource = dynamic_cast<ResourceType *>(iter->second.get())) )
		{
			function(*dyncastResource);
		}
	}
}

template <class ResourceType>
void ResourceManager::ForEach(Function<void(ResourceType &)> function, Function<bool(ResourceType &)> condition)
{
	auto instance = GetInstance();
	ResourceType *dyncastResource = nullptr;

	for ( auto iter = instance->m_Memory.begin(); iter != instance->m_Memory.end(); ++iter )
	{
		if ( (dyncastResource = dynamic_cast<ResourceType *>(iter->second.get())) )
		{
			if ( condition(*dyncastResource) )
			{
				function(*dyncastResource);
			}
		}
	}
}
}
