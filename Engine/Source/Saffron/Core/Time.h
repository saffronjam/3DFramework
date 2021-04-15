#pragma once

namespace Se
{
class Time
{
public:
	Time(float seconds = 0.0f);

	Time& operator=(const Time& rhs);
	Time operator+(const Time& rhs) const;
	Time& operator+=(const Time& rhs);
	Time& operator+=(float seconds);
	Time operator*(float multiplier) const;
	Time& operator*=(float multiplier);
	Time operator/(float multiplier) const;
	Time& operator/=(float multiplier);
	float operator()() const;

	bool operator==(const Time& time) const;
	bool operator>(const Time& time) const;
	bool operator<(const Time& time) const;
	bool operator>=(const Time& time) const;
	bool operator<=(const Time& time) const;

	float sec() const;
	float ms() const;
	float us() const;

	static Time Zero;

private:
	float m_Time;
};
}
