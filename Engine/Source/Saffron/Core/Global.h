#pragma once

#include "Saffron/Core/Timer.h"

namespace Se
{
namespace Global
{
class Timer
{
public:
	using TimePoint = std::chrono::steady_clock::time_point;

public:
	static Time Mark();
	static Time Peek();
	static Time PeekTotal();
	static Time GetStep();
	static void Sync();

	static TimePoint GetStart();
	static TimePoint GetInitialStart();

private:
	static Time m_Ts;
};
};
}
