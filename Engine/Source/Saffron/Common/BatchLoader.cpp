#include "SaffronPCH.h"

#include "Saffron/Common/BatchLoader.h"
#include "Saffron/Common/ScopedLock.h"

namespace Se
{
SignalAggregate<void> BatchLoader::Signals::OnStart;
SignalAggregate<void> BatchLoader::Signals::OnFinish;

BatchLoader::BatchLoader(String name) :
	m_Name(Move(name))
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
	if (m_Worker.joinable())
	{
		m_Worker.join();
	}

	const auto& workerFn = [this]
	{
		ScopedLock queueLock(m_QueueMutex);

		m_Running = true;
		GetSignals().Emit(Signals::OnStart);

		m_Progress = 0.0f;
		for (const auto& [function, shortDescription] : m_Queue)
		{
			if (m_ShouldExit)
			{
				break;
			}
			ScopedLock scopedLock(m_ExecutionMutex);
			m_Status = &shortDescription;
			function();
			m_Progress = m_Progress + 100.0f / static_cast<float>(m_Queue.size());
		}
		m_Progress = 100.0f;
		m_Queue.clear();

		GetSignals().Emit(Signals::OnFinish);
	};

	m_Worker = Thread(workerFn);
}

void BatchLoader::ForceExit()
{
	m_ShouldExit = true;
	if (m_Worker.joinable())
	{
		m_Worker.join();
	}
}

void BatchLoader::Reset()
{
	ScopedLock queueLock(m_QueueMutex);
	ScopedLock scopedLock(m_ExecutionMutex);
	ForceExit();
	m_Queue.clear();
	m_Progress = 0.0f;
	m_ShouldExit = false;
}
}
