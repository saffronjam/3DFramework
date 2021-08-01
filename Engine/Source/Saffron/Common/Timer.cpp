#include "SaffronPCH.h"

#include "Saffron/Common/Timer.h"

namespace Se
{
Timer::Timer() :
	_startTimePoint(Clock::now())
{
}

auto Timer::Restart() -> TimeSpan
{
	const auto now = Clock::now();
	const auto diff = now - _lastTimePoint;
	const long us = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
	_lastFrameTime = TimeSpan::FromMicroseconds(us);
	_lastTimePoint = now;
	return _lastFrameTime;
}

auto Timer::ElapsedTime() const -> TimeSpan
{
	const auto diff = Clock::now() - _lastTimePoint;
	const long us = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
	return TimeSpan::FromMicroseconds(us);
}

auto Timer::FrameTime() const -> TimeSpan
{
	return _lastFrameTime;
}

auto Timer::SinceStart() const -> TimeSpan
{
	const auto diff = Clock::now() - _startTimePoint;
	const long us = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
	return TimeSpan::FromMicroseconds(us);
}
}
