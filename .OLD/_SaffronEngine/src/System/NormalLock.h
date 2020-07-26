#pragma once

#include <mutex>

class NormalLock
{
public:
    void Lock();
    void Unlock();

    bool TryLock()noexcept;

private:
    std::mutex m_mutex;
};

