#pragma once

#include <mutex>

namespace Saffron
{
class NormalLock
{
public:
	void Lock();
	void Unlock();

	bool TryLock()noexcept;

private:
	std::mutex m_mutex;
};
}
