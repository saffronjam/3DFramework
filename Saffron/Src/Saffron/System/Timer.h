#pragma once

#include "Saffron/Config.h"
#include "Saffron/SaffronPCH.h"
#include "Saffron/System/Time.h"

namespace Se
{
class Timer
{
public:
	Timer() ;

	static void UpdateGlobalTimer(float dt);

	Time Mark() ;
	[[nodiscard]] Time Peek() const ;
	static Time PeekGlobal() ;
private:
	std::chrono::steady_clock::time_point m_last;
	static Time m_globalTimer;
};
}


