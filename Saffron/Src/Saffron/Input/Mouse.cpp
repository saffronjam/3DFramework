#include "Saffron/SaffronPCH.h"

#include "Saffron/Input/Mouse.h"

namespace Se
{

Mouse::Mouse()
	:
	m_Position(0.0f, 0.0f),
	m_MoveDelta(0.0f, 0.0f),
	m_IsInScreen(true)
{
}

void Mouse::OnUpdate()
{
	m_PrevButtonMap = m_ButtonMap;
	m_MoveDelta = glm::vec2{ 0.0f, 0.0f };
}

void Mouse::OnEvent(const Event &event)
{
	const EventDispatcher dispathcher(event);
	dispathcher.Try<MousePressEvent>(SE_EVENT_FN(Mouse::OnPress));
	dispathcher.Try<MouseReleaseEvent>(SE_EVENT_FN(Mouse::OnRelease));
	dispathcher.Try<MouseScrollEvent>(SE_EVENT_FN(Mouse::OnScroll));
	dispathcher.Try<MouseMoveEvent>(SE_EVENT_FN(Mouse::OnMove));
	dispathcher.Try<MouseEnterEvent>(SE_EVENT_FN(Mouse::OnEnter));
	dispathcher.Try<MouseLeaveEvent>(SE_EVENT_FN(Mouse::OnLeave));
}

bool Mouse::IsDown(Button button) const
{
	if ( m_ButtonMap.find(button) == m_ButtonMap.end() )
		m_ButtonMap[button] = false;
	return m_ButtonMap[button];
}

bool Mouse::WasDown(Button button) const
{
	if ( m_PrevButtonMap.find(button) == m_PrevButtonMap.end() )
		m_PrevButtonMap[button] = false;
	return m_PrevButtonMap[button];
}

bool Mouse::IsPressed(Button button) const
{
	return IsDown(button) && !WasDown(button);
}

bool Mouse::IsReleased(Button button) const
{
	return !IsDown(button) && WasDown(button);
}

bool Mouse::IsAnyDown() const
{
	for ( auto &[button, state] : m_ButtonMap )
	{
		if ( state )
			return true;
	}
	return false;
}

bool Mouse::IsInScreen() const
{
	return m_IsInScreen;
}

const glm::vec2 &Mouse::GetPosition() const
{
	return m_Position;
}

const glm::vec2 &Mouse::GetDelta() const
{
	return m_MoveDelta;
}

float Mouse::GetHScroll() const
{
	return m_Scroll.x;
}

float Mouse::GetVScroll() const
{
	return m_Scroll.y;
}

void Mouse::OnPress(const MousePressEvent &event)
{
	m_ButtonMap[event.GetButton()] = true;
}

void Mouse::OnRelease(const MouseReleaseEvent &event)
{
	m_ButtonMap[event.GetButton()] = false;
}

void Mouse::OnScroll(const MouseScrollEvent &event)
{
	m_Scroll.x += event.GetOffsetX();
	m_Scroll.y += event.GetOffsetY();
}

void Mouse::OnMove(const MouseMoveEvent &event)
{
	m_MoveDelta = m_Position - event.GetPosition();
	m_Position = event.GetPosition();
}

void Mouse::OnEnter(const MouseEnterEvent &event)
{
}

void Mouse::OnLeave(const MouseLeaveEvent &event)
{
}

}
