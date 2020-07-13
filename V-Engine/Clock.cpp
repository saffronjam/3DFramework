#include "Clock.h"

using namespace std::chrono;

Clock::Clock()
	:
	m_last(steady_clock::now())
{
}

Time Clock::GetDeltatime()
{
	const auto old = m_last;
	m_last = steady_clock::now();
	const duration<double> delta = m_last - old;
	return Time( static_cast<float>( delta.count() ) );
}

Time Clock::PeekDeltatime() const
{
	return duration<float>(steady_clock::now() - m_last).count();
}
