#include "SaffronPCH.h"
#include "Saffron/System/NormalLock.h"

namespace Se
{
void NormalLock::Lock()
{
	m_Mutex.lock();
}

void NormalLock::Unlock()
{
	m_Mutex.unlock();
}

bool NormalLock::TryLock()
{
	return m_Mutex.try_lock();
}
}
