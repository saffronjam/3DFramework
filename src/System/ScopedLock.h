#pragma once

#include <mutex>

class ScopedLock
{
public:
    ScopedLock(std::mutex &mutex);
    ~ScopedLock();

private:
    std::mutex &m_mutex;
};


