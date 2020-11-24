#include "SaffronPCH.h"
#include "ResourceManager.h"

namespace Se
{
Shared<Resource> &ResourceManager::Get(Shared<Resource> resource)
{
	return Get(resource->GetIdentifier());
}

Shared<Resource> &ResourceManager::Get(size_t identifer)
{
	return GetInstance()->m_Memory.at(identifer);
}

bool ResourceManager::Exists(Shared<Resource> resource)
{
	return Exists(resource->GetIdentifier());
}

bool ResourceManager::Exists(size_t identifier)
{
	auto &instance = GetInstance();
	return instance->m_Memory.find(identifier) != instance->m_Memory.end();
}

void ResourceManager::Emplace(Shared<Resource> resource)
{
	auto &instance = GetInstance();
	instance->m_Memory.emplace(resource->GetIdentifier(), resource);
}

const ArrayList<Shared<Resource>> &ResourceManager::GetAll()
{
	auto &instance = GetInstance();
	if ( instance->m_NeedCacheSync )
	{
		instance->SyncCache();
	}
	return GetInstance()->m_ReturnCache;
}

Shared<ResourceManager> &ResourceManager::GetInstance()
{
	static Shared<ResourceManager> s_ResourceManager = Shared<ResourceManager>::Create();
	return s_ResourceManager;
}

void ResourceManager::SyncCache()
{
	m_ReturnCache.clear();
	m_ReturnCache.reserve(m_Memory.size());
	std::transform(m_Memory.begin(), m_Memory.end(), std::back_inserter(m_ReturnCache),
				   [](const auto &pair) { return pair.second; });
	m_NeedCacheSync = false;
}
}
