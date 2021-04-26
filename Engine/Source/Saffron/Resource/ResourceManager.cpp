#include "SaffronPCH.h"

#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Resource/ResourceManager.h"

namespace Se
{
ResourceManager::ResourceManager() :
	SingleTon(this)
{
}

const Shared<Resource>& ResourceManager::Get(const Shared<Resource>& resource)
{
	return Get(resource->GetResourceID());
}

const Shared<Resource>& ResourceManager::Get(ulong identifier)
{
	auto& instance = Instance();
	Debug::Assert(Contains(identifier), "No resource registered with identifier: {0}", identifier);
	return Instance()._resources.at(identifier);
}

const Shared<Resource>& ResourceManager::GetFallback(const String& identifier)
{
	auto& instance = Instance();
	Debug::Assert(ContainsFallback(identifier), "No fallback registered with identifier: {0}", identifier);
	return _instance->_fallbacks.at(identifier);
}

bool ResourceManager::Contains(const Shared<Resource>& resource)
{
	return Contains(resource->GetResourceID());
}

bool ResourceManager::Contains(ulong identifier)
{
	auto& instance = Instance();
	return instance._resources.find(identifier) != instance._resources.end();
}

bool ResourceManager::ContainsFallback(const String& identifier)
{
	auto& instance = Instance();
	return instance._fallbacks.find(identifier) != instance._fallbacks.end();
}

void ResourceManager::Add(Shared<Resource> resource)
{
	auto& instance = Instance();
	instance._resources.emplace(resource->GetResourceID(), Move(resource));
}

void ResourceManager::Add(Shared<Resource> resource, ulong identifier)
{
	auto& instance = Instance();
	instance._resources.emplace(identifier, Move(resource));
}

void ResourceManager::AddFallback(Shared<Resource> resource, String identifier)
{
	Instance()._fallbacks.emplace(Move(identifier), Move(resource));
}

const ArrayList<Shared<Resource>>& ResourceManager::GetAll()
{
	auto& instance = Instance();
	if (instance._needCacheSync)
	{
		instance.SyncCache();
	}
	return Instance()._resourceReturnCache;
}

void ResourceManager::Clear()
{
	auto& instance = Instance();
	instance._resources.clear();
	instance._resourceReturnCache.clear();
	instance._needCacheSync = false;
}

void ResourceManager::SyncCache()
{
	_resourceReturnCache.clear();
	_resourceReturnCache.reserve(_resources.size());
	std::transform(_resources.begin(), _resources.end(), std::back_inserter(_resourceReturnCache),
	               [](const auto& pair) { return pair.second; });
	_needCacheSync = false;
}
}
