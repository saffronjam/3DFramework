#pragma once

#include <chrono>

#include "Time.h"

class Clock
{
public:
	Clock();

	Time GetDeltatime();
	Time PeekDeltatime() const;

private:
	std::chrono::steady_clock::time_point m_last;
};
