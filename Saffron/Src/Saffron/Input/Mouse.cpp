#include "Saffron/SaffronPCH.h"

#include "Saffron/Input/Mouse.h"

namespace Se
{

Mouse::Mouse()
	:
	m_position(0.0f, 0.0f),
	m_moveDelta(0.0f, 0.0f),
	m_isInScreen(true)
{
}

void Mouse::OnUpdate()
{
	m_prevButtonmap = m_buttonmap;
	m_moveDelta = glm::vec2{ 0.0f, 0.0f };
}

void Mouse::OnEvent(const Event &event)
{
	const EventDispatcher dispathcher(event);
	dispathcher.Try<MousePressEvent>(SE_EVENT_FN(Mouse::OnPress));
	dispathcher.Try<MouseReleaseEvent>(SE_EVENT_FN(Mouse::OnRelease));
	dispathcher.Try<MouseMoveEvent>(SE_EVENT_FN(Mouse::OnMove));
	dispathcher.Try<MouseEnterEvent>(SE_EVENT_FN(Mouse::OnEnter));
	dispathcher.Try<MouseLeaveEvent>(SE_EVENT_FN(Mouse::OnLeave));
}

bool Mouse::IsDown(Button button) const
{
	if ( m_buttonmap.find(button) == m_buttonmap.end() )
		m_buttonmap[button] = false;
	return m_buttonmap[button];
}

bool Mouse::WasDown(Button button) const
{
	if ( m_prevButtonmap.find(button) == m_prevButtonmap.end() )
		m_prevButtonmap[button] = false;
	return m_prevButtonmap[button];
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
	for ( auto &[button, state] : m_buttonmap )
	{
		if ( state )
			return true;
	}
	return false;
}

bool Mouse::IsInScreen() const
{
	return m_isInScreen;
}

const glm::vec2 &Mouse::GetPosition() const
{
	return m_position;
}

const glm::vec2 &Mouse::GetDelta() const
{
	return m_moveDelta;
}

void Mouse::OnPress(const MousePressEvent &event)
{
	m_buttonmap[event.GetButton()] = true;
}

void Mouse::OnRelease(const MouseReleaseEvent &event)
{
	m_buttonmap[event.GetButton()] = false;
}

void Mouse::OnMove(const MouseMoveEvent &event)
{
	m_moveDelta = m_position - event.GetPosition();
	m_position = event.GetPosition();
}

void Mouse::OnEnter(const MouseEnterEvent &event)
{
}

void Mouse::OnLeave(const MouseLeaveEvent &event)
{
}

}
