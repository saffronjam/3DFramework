#include "NormalLock.h"

namespace Saffron
{
void NormalLock::Lock()
{
	m_mutex.lock();
}

void NormalLock::Unlock()
{
	m_mutex.unlock();
}

bool NormalLock::TryLock() noexcept
{
	return m_mutex.try_lock();
}
}
