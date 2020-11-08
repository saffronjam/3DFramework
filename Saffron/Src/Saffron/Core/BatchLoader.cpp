#include "SaffronPCH.h"

#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Core/ScopedLock.h"

namespace Se
{

BatchLoader::BatchLoader(String name)
	: m_Name(Move(name))
{
}

BatchLoader::~BatchLoader()
{
	ForceExit();
}

void BatchLoader::Submit(Function<void()> function, String shortDescription)
{
	ScopedLock queueLock(m_QueueMutex);
	m_Queue.emplace_back(Move(function), Move(shortDescription));
}

void BatchLoader::Execute()
{
	ScopedLock queueLock(m_QueueMutex);
	if ( m_Worker.joinable() )
	{
		m_Worker.join();
	}
	m_Worker = Thread([this]
					  {
						  ScopedLock queueLock(m_QueueMutex);

						  m_Running = true;
						  if ( m_OnStart )
							  m_OnStart();

						  m_Progress = 0.0f;
						  for ( const auto &[function, shortDescription] : m_Queue )
						  {
							  if ( m_ShouldExit )
							  {
								  break;
							  }
							  ScopedLock scopedLock(m_ExecutionMutex);
							  if ( m_OnEachExecution )
								  m_OnEachExecution();
							  m_Status = &shortDescription;
							  function();
							  m_Progress = m_Progress + 100.0f / static_cast<float>(m_Queue.size());
						  }
						  m_Progress = 100.0f;
						  m_Queue.clear();

						  if ( m_OnFinish )
							  m_OnFinish();
					  });
}

void BatchLoader::ForceExit()
{
	m_ShouldExit = true;
	if ( m_Worker.joinable() )
	{
		m_Worker.join();
	}
}

}
