#pragma once

namespace Saffron
{
class Time
{
public:
	Time(float time);

	Time &operator+(const Time &rhs);
	Time &operator+=(const Time &rhs);
	float operator()() const;

	float Sec();
	float MS();

private:
	float m_Time;

};
}
