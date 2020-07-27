#pragma once

#include <mutex>

namespace Saffron
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

