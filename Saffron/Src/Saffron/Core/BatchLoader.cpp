#include "SaffronPCH.h"

#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Core/ScopedLock.h"

namespace Se
{
static Unique<BatchLoader> s_Preloader = Unique<BatchLoader>::Create("Preloader");

BatchLoader::BatchLoader(String name)
	: m_Name(Move(name))
{
}

void BatchLoader::Submit(Function<void()> function, String shortDescription)
{
	ScopedLock queueLock(m_QueueMutex);
	m_Queue.emplace_back(Move(function), Move(shortDescription));
}

void BatchLoader::Execute()
{
	ScopedLock queueLock(m_QueueMutex);
	m_Progress = 0.0f;
	for ( const auto &[function, shortDescription] : m_Queue )
	{
		//ScopedLock scopedLock(m_ExecutionMutex);
		if ( m_OnEachExecution )
			m_OnEachExecution();
		m_Status = &shortDescription;
		function();
		m_Progress = m_Progress + 100.0f / static_cast<float>(m_Queue.size());
	}
	m_Progress = 100.0f;
	if ( m_OnFinish )
		m_OnFinish();
	m_Queue.clear();
}

Unique<BatchLoader> &BatchLoader::GetPreloader()
{
	SE_CORE_ASSERT(s_Preloader, "Preloader is only valid prior to running an application (Used for the splash screen)");
	return s_Preloader;
}

void BatchLoader::InvalidatePreloader()
{
	s_Preloader.Reset();
}
}
