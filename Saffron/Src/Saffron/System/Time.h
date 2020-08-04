#pragma once

namespace Se
{
class Time
{
public:
	Time(float seconds = 0.0f);

	Time &operator+(const Time &rhs);
	Time &operator+=(const Time &rhs);
	Time &operator+=(float seconds);
	float operator()() const;

	float sec() const;
	float ms() const;
	float us() const;

private:
	float m_Time;

};
}
