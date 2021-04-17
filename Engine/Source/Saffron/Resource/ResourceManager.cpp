#include "SaffronPCH.h"
#include "ResourceManager.h"

namespace Se
{
Shared<Resource>& ResourceManager::Get(Shared<Resource> resource)
{
	return Get(resource->GetResourceID());
}

Shared<Resource>& ResourceManager::Get(size_t identifer)
{
	return GetInstance()->_memory.at(identifer);
}

bool ResourceManager::Exists(Shared<Resource> resource)
{
	return Exists(resource->GetResourceID());
}

bool ResourceManager::Exists(size_t identifier)
{
	auto& instance = GetInstance();
	return instance->_memory.find(identifier) != instance->_memory.end();
}

void ResourceManager::Emplace(Shared<Resource> resource)
{
	auto& instance = GetInstance();
	instance->_memory.emplace(resource->GetResourceID(), resource);
}

void ResourceManager::Emplace(Shared<Resource> resource, size_t identifier)
{
	auto& instance = GetInstance();
	instance->_memory.emplace(identifier, resource);
}

const ArrayList<Shared<Resource>>& ResourceManager::GetAll()
{
	auto& instance = GetInstance();
	if (instance->_needCacheSync)
	{
		instance->SyncCache();
	}
	return GetInstance()->_returnCache;
}

void ResourceManager::Clear()
{
	GetInstance()->_memory.clear();
	GetInstance()->_returnCache.clear();
	GetInstance()->_needCacheSync = false;
}

Shared<ResourceManager>& ResourceManager::GetInstance()
{
	static Shared<ResourceManager> s_ResourceManager = Shared<ResourceManager>::Create();
	return s_ResourceManager;
}

void ResourceManager::SyncCache()
{
	_returnCache.clear();
	_returnCache.reserve(_memory.size());
	std::transform(_memory.begin(), _memory.end(), std::back_inserter(_returnCache),
	               [](const auto& pair) { return pair.second; });
	_needCacheSync = false;
}
}
