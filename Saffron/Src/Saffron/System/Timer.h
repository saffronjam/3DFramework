#pragma once

#include "Saffron/Config.h"
#include "Saffron/SaffronPCH.h"
#include "Saffron/System/Time.h"

namespace Se
{
class Timer
{
public:
	Timer() noexcept;

	static void UpdateGlobalTimer(float dt);

	Time Mark() noexcept;
	[[nodiscard]] Time Peek() const noexcept;
	static Time PeekGlobal() noexcept;
private:
	std::chrono::steady_clock::time_point m_last;
	static Time m_globalTimer;
};
}


