#include "SaffronPCH.h"

#include "Saffron/Core/Global.h"

namespace Se::Global
{
Se::Timer s_Timer = Se::Timer("Global timer");
Time Timer::m_Ts = Time::Zero();

Time Timer::Mark()
{
	m_Ts = s_Timer.Mark();
	return m_Ts;
}

Time Timer::Peek()
{
	return s_Timer.Peek();
}

Time Timer::PeekTotal()
{
	return s_Timer.PeekTotal();
}

Time Timer::GetStep()
{
	return m_Ts;
}

void Timer::Sync()
{
	s_Timer.Sync();
}

Timer::TimePoint Timer::GetStart()
{
	return s_Timer.GetStart();
}

Timer::TimePoint Timer::GetInitialStart()
{
	return s_Timer.GetInitialStart();
}
}
