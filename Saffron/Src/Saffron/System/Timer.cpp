#include "Saffron/SaffronPCH.h"
#include "Saffron/System/Timer.h"

namespace Se
{

using namespace std::chrono;

Time Timer::m_globalTimer(0.0f);

Timer::Timer()
	: m_last(steady_clock::now())
{
}

void Timer::UpdateGlobalTimer(float dt)
{
	m_globalTimer += dt;
}

Time Timer::Mark()
{
	const auto old = m_last;
	m_last = steady_clock::now();
	const duration<float> frameTime = m_last - old;
	return Time(frameTime.count());
}

Time Timer::Peek() const
{
	return Time(duration<float>(steady_clock::now() - m_last).count());
}

Time Timer::PeekGlobal()
{
	return m_globalTimer;
}

}
