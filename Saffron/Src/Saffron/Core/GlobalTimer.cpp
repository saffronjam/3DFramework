#include "SaffronPCH.h"

#include "Saffron/Core/GlobalTimer.h"

namespace Se
{
Timer s_Timer = Timer("Global timer");
Time GlobalTimer::m_Ts = Time::Zero();

Time GlobalTimer::Mark()
{
	m_Ts = s_Timer.Mark();
	return m_Ts;
}

Time GlobalTimer::Peek()
{
	return s_Timer.Peek();
}

Time GlobalTimer::PeekTotal()
{
	return s_Timer.PeekTotal();
}

Time GlobalTimer::GetStep()
{
	return m_Ts;
}

Timer::TimePoint GlobalTimer::GetStart()
{
	return s_Timer.GetStart();
}

Timer::TimePoint GlobalTimer::GetInitialStart()
{
	return s_Timer.GetInitialStart();
}
}
