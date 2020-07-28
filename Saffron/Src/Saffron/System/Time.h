#pragma once

#include "Saffron/Config.h"

namespace Se
{
class Time
{
public:
	Time(float time);

	Time &operator+(const Time &rhs);
	Time &operator+=(const Time &rhs);
	float operator()() const;

	float sec() const;
	float ms() const;

private:
	float m_Time;

};
}
