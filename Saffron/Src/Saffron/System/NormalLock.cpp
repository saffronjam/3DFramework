#include "SaffronPCH.h"
#include "Saffron/System/NormalLock.h"

namespace Se
{
void NormalLock::Lock()
{
	m_mutex.lock();
}

void NormalLock::Unlock()
{
	m_mutex.unlock();
}

bool NormalLock::TryLock() 
{
	return m_mutex.try_lock();
}
}
