#pragma once

#include "Saffron/Core/Timer.h"

namespace Se
{
class GlobalTimer
{
public:
	using TimePoint = std::chrono::steady_clock::time_point;

public:
	static Time Mark();
	static Time Peek();
	static Time PeekTotal();
	static Time GetStep();

	static TimePoint GetStart();
	static TimePoint GetInitialStart();

private:
	static Time m_Ts;
};
}