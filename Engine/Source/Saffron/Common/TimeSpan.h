#pragma once

namespace Se
{
class TimeSpan
{
public:
	float Sec();
	float Ms();
	double Us();

	static auto FromSeconds(float seconds) -> TimeSpan;
	static auto FromMilliseconds(float milliseconds) -> TimeSpan;
	static auto FromMicroseconds(double microseconds) -> TimeSpan;

	static const TimeSpan Zero;

private:
	explicit TimeSpan(double microSeconds);

private:
	double _usTicks;
};
}
