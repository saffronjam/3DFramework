#pragma once

class Seconds
{
	friend class Time;
public:
	Seconds( float amount ) : m_amount( amount ) {}

private:
	float m_amount;
};

class Milliseconds
{
	friend class Time;
public:
	Milliseconds( float amount ) : m_amount( amount ) {}

private:
	float m_amount;
};

class Microseconds
{
	friend class Time;
public:
	Microseconds( float amount ) : m_amount( amount ) {}

private:
	float m_amount;
};


class Time
{
	friend class Clock;
public:
	Time( float delta = 0.0 ) : m_count( delta ) {}

	void operator+( const Seconds &rhs ) { m_count += rhs.m_amount; }
	void operator+( const Milliseconds &rhs ) { m_count += rhs.m_amount; }
	void operator+( const Microseconds &rhs ) { m_count += rhs.m_amount; }

	void operator-( const Seconds &rhs ) { m_count -= rhs.m_amount; }
	void operator-( const Milliseconds &rhs ) { m_count -= rhs.m_amount; }
	void operator-( const Microseconds &rhs ) { m_count -= rhs.m_amount; }

	[[nodiscard]] float AsSeconds() const noexcept { return m_count; }
	[[nodiscard]] float AsMilliseconds() const noexcept { return m_count * 10e3f; }
	[[nodiscard]] float AsMicroseconds() const noexcept { return m_count * 10e6f; }

private:
	// Value measured in seconds
	float m_count;
};

