#include "Clock.h"

using namespace std::chrono;

ve::Clock::Clock()
	:
	m_last(steady_clock::now())
{
}

ve::Time ve::Clock::GetDeltatime()
{
	const auto old = m_last;
	m_last = steady_clock::now();
	const duration<double> delta = m_last - old;
	return Time(delta.count());
}

ve::Time ve::Clock::PeekDeltatime() const
{
	return duration<double>(steady_clock::now() - m_last).count();
}
