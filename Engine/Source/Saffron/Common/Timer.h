#pragma once

#include <chrono>

#include "Saffron/Base.h"
#include "Saffron/Common/TimeSpan.h"

namespace Se
{
class Timer
{
	using Clock = std::chrono::high_resolution_clock;

public:
	Timer();

	auto Restart() -> TimeSpan;

	auto ElapsedTime() const -> TimeSpan;
	auto FrameTime() const -> TimeSpan;
	auto SinceStart() const -> TimeSpan;

private:
	Clock::time_point _startTimePoint;
	Clock::time_point _lastTimePoint;
	TimeSpan _lastFrameTime = TimeSpan::Zero;
};
}
