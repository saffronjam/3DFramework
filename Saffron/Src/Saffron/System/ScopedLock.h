#pragma once

#include "Saffron/Base.h"

namespace Se
{
class ScopedLock
{
public:
	ScopedLock(Mutex &mutex);
	~ScopedLock();

private:
	Mutex &m_Mutex;
};
}

