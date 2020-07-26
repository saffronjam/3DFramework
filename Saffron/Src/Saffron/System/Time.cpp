#include "Saffron/System/Time.h"

namespace Saffron
{

Time::Time(float time)
	: m_Time(time)
{
}

Time &Time::operator+(const Time &rhs)
{
	return Time(m_Time + rhs.m_Time);
}

Time &Time::operator+=(const Time &rhs)
{
	return *this = *this + rhs;
}

float Time::operator()() const
{
	return m_Time;
}

float Time::Sec()
{
	return m_Time;
}

float Time::MS()
{
	return m_Time * 1000.0f;
}

}