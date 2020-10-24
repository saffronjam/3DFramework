#pragma once

#include "Saffron/Base.h"

namespace Se
{
class NormalLock
{
public:
	void Lock();
	void Unlock();

	bool TryLock();

private:
	Mutex m_Mutex;
};
}
