#include "Mouse.h"
#include "VeWin.h"

void Mouse::Update()
{
	m_prevState = m_state;
}

bool Mouse::LeftIsDown() const noexcept
{
	return m_state.first;
}

bool Mouse::RightIsDown() const noexcept
{
	return m_state.second;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return m_state.first && !m_prevState.first;
}

bool Mouse::RightIsPressed() const noexcept
{
	return m_state.second && !m_prevState.second;
}

bool Mouse::LeftIsReleased() const noexcept
{
	return !m_state.first && m_prevState.first;
}

bool Mouse::RightIsReleased() const noexcept
{
	return !m_state.second && m_prevState.second;
}

std::optional<Mouse::Event> Mouse::Read() noexcept
{
	if ( m_buffer.size() > 0u )
	{
		Event e = m_buffer.front();
		m_buffer.pop();
		return e;
	}
	else
	{
		return std::nullopt;
	}
}

bool Mouse::IsEmpty() const noexcept
{
	return m_buffer.empty();
}

void Mouse::Clear() noexcept
{
	m_buffer = std::queue<Event>();
}

void Mouse::OnMove(int x, int y) noexcept
{
	m_coords.first = x;
	m_coords.second = y;

	m_buffer.push(Event(Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnLeave(int x, int y) noexcept
{
	m_inWindow = false;
	m_buffer.push(Event(Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnEnter(int x, int y) noexcept
{
	m_inWindow = true;
	m_buffer.push(Event(Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnLeftPress(int x, int y) noexcept
{
	m_state.first = true;
}

void Mouse::OnLeftRelease(int x, int y) noexcept
{
	m_state.first = false;
}

void Mouse::OnRightPress(int x, int y) noexcept
{
	m_state.second = true;
}

void Mouse::OnRightRelease(int x, int y) noexcept
{
	m_state.second = false;
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	m_buffer.push(Event(Event::Type::WheelUp, *this));
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	m_buffer.push(Event(Event::Type::WheelDown, *this));
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	m_wheelDeltaCarry += delta;
	while ( m_wheelDeltaCarry > WHEEL_DELTA )
	{
		m_wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while ( m_wheelDeltaCarry < -WHEEL_DELTA )
	{
		m_wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

void Mouse::TrimBuffer() noexcept
{
	while ( m_buffer.size() > m_bufferSize )
		m_buffer.pop();
}
