#include "SaffronPCH.h"
#include "Saffron/System/ScopedLock.h"

namespace Se
{
ScopedLock::ScopedLock(Mutex &mutex)
	: m_Mutex(mutex)
{
	m_Mutex.lock();
}

ScopedLock::~ScopedLock()
{
	m_Mutex.unlock();
}
}
