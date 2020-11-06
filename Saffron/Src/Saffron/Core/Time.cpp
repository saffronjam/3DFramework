#include "SaffronPCH.h"

#include "Saffron/Core/Time.h"

namespace Se
{

Time::Time(float seconds)
	: m_Time(seconds)
{
}

Time &Time::operator=(const Time &rhs)
{
	m_Time = rhs.m_Time;
	return *this;
}

Time Time::operator+(const Time &rhs)const
{
	return { m_Time + rhs.m_Time };
}

Time &Time::operator+=(const Time &rhs)
{
	return *this = *this + rhs;
}

Time &Time::operator+=(float seconds)
{
	m_Time += seconds;
	return *this;
}

Time Time::operator*(float multiplier) const
{
	return { m_Time * multiplier };
}

Time &Time::operator*=(float multiplier)
{
	return *this = *this * multiplier;
}

Time Time::operator/(float multiplier) const
{
	return { m_Time / multiplier };
}

Time &Time::operator/=(float multiplier)
{
	return *this = *this / multiplier;
}

float Time::operator()() const
{
	return m_Time;
}

bool Time::operator==(const Time &time) const
{
	return m_Time == time.m_Time;
}

bool Time::operator>(const Time &time) const
{
	return m_Time > time.m_Time;
}

bool Time::operator<(const Time &time) const
{
	return m_Time < time.m_Time;
}

bool Time::operator>=(const Time &time) const
{
	return m_Time >= time.m_Time;
}

bool Time::operator<=(const Time &time) const
{
	return m_Time <= time.m_Time;
}

float Time::sec() const
{
	return m_Time;
}

float Time::ms() const
{
	return m_Time * 1000.0f;
}

float Time::us() const
{
	return m_Time * 1000000.0f;
}
}
