#include "SaffronPCH.h"

#include "Saffron/Common/TimeSpan.h"

namespace Se
{
const TimeSpan TimeSpan::Zero(0l);

float TimeSpan::Sec()
{
	return static_cast<float>(_usTicks) / 10e6f;
}

float TimeSpan::Ms()
{
	return _usTicks / 10e3;
}

double TimeSpan::Us()
{
	return _usTicks;
}

auto TimeSpan::FromSeconds(float seconds) -> TimeSpan
{
	return TimeSpan(seconds * 10e6f);
}

auto TimeSpan::FromMilliseconds(float milliseconds) -> TimeSpan
{
	return TimeSpan(milliseconds * 10e3f);
}

auto TimeSpan::FromMicroseconds(double microseconds) -> TimeSpan
{
	return TimeSpan(microseconds);
}

TimeSpan::TimeSpan(double microSeconds) :
	_usTicks(microSeconds)
{
}
}
