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
	Time &operator*(float multiplier);
	Time &operator*=(float multiplier);
	float operator()() const;

	float sec() const;
	float ms() const;
	float us() const;

	static Time Zero() { return Time(0.0f); }

private:
	float m_Time;

};
}
