#pragma once

#include <cinttypes>

namespace ve
{
class Time
{
	friend class Clock;
public:
	Time(double delta = 0.0);

	double asMicroseconds() const;
	double asMilliseconds() const;
	double asSeconds() const;

	void AddSeconds(const double seconds);
	void AddMilliseconds(const double milliseconds);
	void AddMicroseconds(const double microseconds);
private:
	double m_currentDelta;
};
}
