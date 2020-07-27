#pragma once

#include <mutex>

namespace Se
{
class ScopedLock
{
public:
	ScopedLock(std::mutex &mutex);
	~ScopedLock();

private:
	std::mutex &m_mutex;
};
}

