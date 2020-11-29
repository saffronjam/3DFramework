#pragma once

#include "Saffron/Core/MemManaged.h"
#include "Saffron/Resource/Resource.h"

namespace Se
{
class ResourceManager : public MemManaged<ResourceManager>
{
public:
	static std::shared_ptr<Resource> &Get(std::shared_ptr<Resource> resource);
	static std::shared_ptr<Resource> &Get(size_t identifer);
	static bool Exists(std::shared_ptr<Resource> resource);
	static bool Exists(size_t identifier);
	static void Emplace(std::shared_ptr<Resource> resource);
	static void Emplace(std::shared_ptr<Resource> resource, size_t identifier);
	static const ArrayList<std::shared_ptr<Resource>> &GetAll();
	static void Clear();

	template<class ResourceType>
	static void ForEach(Function<void(ResourceType &)> function);
	template<class ResourceType>
	static void ForEach(Function<void(ResourceType &)> function, Function<bool(ResourceType &)> condition);

private:
	static std::shared_ptr<ResourceManager> &GetInstance();
	void SyncCache();

private:
	Map<size_t, std::shared_ptr<Resource>> m_Memory;
	ArrayList<std::shared_ptr<Resource>> m_ReturnCache;
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
