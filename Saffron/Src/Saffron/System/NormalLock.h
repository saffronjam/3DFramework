#pragma once

#include <mutex>

namespace Se
{
class NormalLock
{
public:
	void Lock();
	void Unlock();

	bool TryLock();

private:
	std::mutex m_mutex;
};
}
