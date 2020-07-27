#include "Saffron/SaffronPCH.h"
#include "Saffron/System/Time.h"

namespace Se
{

Time::Time(float time)
	: m_Time(time)
{
}

Time &Time::operator+(const Time &rhs)
{
	m_Time += rhs.m_Time;
	return *this;
}

Time &Time::operator+=(const Time &rhs)
{
	return *this = *this + rhs;
}

float Time::operator()() const
{
	return m_Time;
}

float Time::sec() const
{
	return m_Time;
}

float Time::ms() const
{
	return m_Time * 1000.0f;
}

}