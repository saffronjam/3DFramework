#include "ScopedLock.h"

ScopedLock::ScopedLock(std::mutex &mutex)
        : m_mutex(mutex)
{
    m_mutex.lock();
}

ScopedLock::~ScopedLock()
{
    m_mutex.unlock();
}
