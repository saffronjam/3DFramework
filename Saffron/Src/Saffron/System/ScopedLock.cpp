#include "SaffronPCH.h"
#include "Saffron/System/ScopedLock.h"

namespace Se
{
ScopedLock::ScopedLock(std::mutex &mutex)
	: m_mutex(mutex)
{
	m_mutex.lock();
}

ScopedLock::~ScopedLock()
{
	m_mutex.unlock();
}
}
