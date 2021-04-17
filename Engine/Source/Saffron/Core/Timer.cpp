#include "SaffronPCH.h"

#include "Saffron/Core/Timer.h"

namespace Se
{
using namespace std::chrono;

Timer::Timer(const char* name) :
	_name(name),
	_lastTimePoint(steady_clock::now()),
	_initialTimePoint(steady_clock::now())
{
}

Time Timer::Mark()
{
	const auto old = _lastTimePoint;
	_lastTimePoint = steady_clock::now();
	return Time(duration<float>(_lastTimePoint - old).count());
}

Time Timer::Peek() const
{
	return Time(duration<float>(steady_clock::now() - _lastTimePoint).count());
}

Time Timer::PeekTotal() const
{
	return Time(duration<float>(steady_clock::now() - _initialTimePoint).count());
}

void Timer::Sync()
{
	_lastTimePoint = steady_clock::now();
}

const char* Timer::GetName()
{
	return _name;
}

Timer::TimePoint Timer::GetStart() const
{
	return _lastTimePoint;
}

Timer::TimePoint Timer::GetInitialStart() const
{
	return _initialTimePoint;
}
}
