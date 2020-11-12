#pragma once

#include "Saffron/Base.h"

namespace Se
{

class DateTime
{
	using TimePoint = std::chrono::system_clock::time_point;

public:
	struct Date
	{
		int Seconds;
		int Minutes;
		int Hour;
		int Weekday;
		int Day;
		int Month;
		int Year;
	};

public:
	DateTime();
	explicit DateTime(Date date);
	explicit DateTime(TimePoint timePoint);
	DateTime(int year, int month, int day, int weekday, int hour, int minutes, int seconds);

	bool operator<(const DateTime &rhs) const;
	bool operator>(const DateTime &rhs) const;

	int Seconds() const { return m_Date.Seconds; }
	int Minutes() const { return m_Date.Minutes; }
	int Hour() const { return m_Date.Hour; }
	int Weekday() const { return m_Date.Weekday; }
	int Day() const { return m_Date.Day; }
	int Month() const { return m_Date.Month; }
	int Year() const { return m_Date.Year; }

	String WeekdayString(bool abbreviation = false) const;
	String MonthString(bool abbreviation = false) const;
	String TimeString() const;
	String ANSIDateString() const;


private:
	void Clamp();

private:
	Date m_Date{};
};

}
