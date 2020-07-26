#include "ScopedLock.h"

namespace Saffron
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
