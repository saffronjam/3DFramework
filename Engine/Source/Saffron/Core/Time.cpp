#include "SaffronPCH.h"

#include "Saffron/Core/Time.h"

namespace Se
{
Time Time::Zero = Time(0.0f);

Time::Time(float seconds) :
	_time(seconds)
{
}

Time& Time::operator=(const Time& rhs)
{
	_time = rhs._time;
	return *this;
}

Time Time::operator+(const Time& rhs) const
{
	return {_time + rhs._time};
}

Time& Time::operator+=(const Time& rhs)
{
	return *this = *this + rhs;
}

Time& Time::operator+=(float seconds)
{
	_time += seconds;
	return *this;
}

Time Time::operator*(float multiplier) const
{
	return {_time * multiplier};
}

Time& Time::operator*=(float multiplier)
{
	return *this = *this * multiplier;
}

Time Time::operator/(float multiplier) const
{
	return {_time / multiplier};
}

Time& Time::operator/=(float multiplier)
{
	return *this = *this / multiplier;
}

float Time::operator()() const
{
	return _time;
}

bool Time::operator==(const Time& time) const
{
	return _time == time._time;
}

bool Time::operator>(const Time& time) const
{
	return _time > time._time;
}

bool Time::operator<(const Time& time) const
{
	return _time < time._time;
}

bool Time::operator>=(const Time& time) const
{
	return _time >= time._time;
}

bool Time::operator<=(const Time& time) const
{
	return _time <= time._time;
}

float Time::sec() const
{
	return _time;
}

float Time::ms() const
{
	return _time * 1000.0f;
}

float Time::us() const
{
	return _time * 1000000.0f;
}
}
