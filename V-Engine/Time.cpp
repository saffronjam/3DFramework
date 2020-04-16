#include "Time.h"
#include "Time.h"

ve::Time::Time(double delta)
	:
	m_currentDelta(delta)
{
}

double ve::Time::asMicroseconds() const
{
	return m_currentDelta * 1000000.0;
}

double ve::Time::asMilliseconds() const
{
	return m_currentDelta * 1000.0;
}

double ve::Time::asSeconds() const
{
	return m_currentDelta;
}

void ve::Time::AddSeconds(const double seconds)
{
	m_currentDelta += seconds;
}

void ve::Time::AddMilliseconds(const double milliseconds)
{
	m_currentDelta += milliseconds;
}

void ve::Time::AddMicroseconds(const double microseconds)
{
	m_currentDelta += microseconds;
}
