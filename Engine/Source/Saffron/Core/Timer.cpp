#include "SaffronPCH.h"

#include "Saffron/Core/Timer.h"

namespace Se
{
using namespace std::chrono;

Timer::Timer(const char* name) :
	m_Name(name),
	m_LastTimePoint(steady_clock::now()),
	m_InitialTimePoint(steady_clock::now())
{
}

Time Timer::Mark()
{
	const auto old = m_LastTimePoint;
	m_LastTimePoint = steady_clock::now();
	return Time(duration<float>(m_LastTimePoint - old).count());
}

Time Timer::Peek() const
{
	return Time(duration<float>(steady_clock::now() - m_LastTimePoint).count());
}

Time Timer::PeekTotal() const
{
	return Time(duration<float>(steady_clock::now() - m_InitialTimePoint).count());
}

void Timer::Sync()
{
	m_LastTimePoint = steady_clock::now();
}

const char* Timer::GetName()
{
	return m_Name;
}

Timer::TimePoint Timer::GetStart() const
{
	return m_LastTimePoint;
}

Timer::TimePoint Timer::GetInitialStart() const
{
	return m_InitialTimePoint;
}
}
