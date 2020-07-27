#include "Timer.h"

namespace Saffron
{

using namespace std::chrono;

Time Timer::m_globalTimer(0.0f);

Timer::Timer() noexcept
	: m_last(steady_clock::now())
{
}

void Timer::UpdateGlobalTimer(float dt)
{
	m_globalTimer += dt;
}

Time Timer::Mark() noexcept
{
	const auto old = m_last;
	m_last = steady_clock::now();
	const duration<float> frameTime = m_last - old;
	return frameTime.count();
}

Time Timer::Peek() const noexcept
{
	return duration<float>(steady_clock::now() - m_last).count();
}

Time Timer::PeekGlobal() noexcept
{
	return m_globalTimer;
}

}
