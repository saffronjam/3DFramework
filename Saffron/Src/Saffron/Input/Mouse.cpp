#include "Saffron/Input/Mouse.h"
#include "Saffron/Graphics/Window.h"

namespace Saffron
{

Mouse::Mouse(Window &window)
	:
	m_wnd(window),
	m_position(0.0f, 0.0f),
	m_moveDelta(0.0f, 0.0f),
	m_isInScreen(true)
{
	window.AddEventHandler(this);
}

void Mouse::Update() noexcept
{
	m_prevButtonmap = m_buttonmap;
	m_moveDelta = glm::vec2{ 0.0f, 0.0f };
}

#include <functional>

void Mouse::OnEvent(const Event::Ptr pEvent)
{
	const EventDispatcher dispathcher(pEvent);
	dispathcher.Try<MousePressEvent>(EVENT_FN(Mouse::OnPress));
	dispathcher.Try<MouseReleaseEvent>(EVENT_FN(Mouse::OnRelease));
	dispathcher.Try<MouseMoveEvent>(EVENT_FN(Mouse::OnMove));
	dispathcher.Try<MouseEnterEvent>(EVENT_FN(Mouse::OnEnter));
	dispathcher.Try<MouseLeaveEvent>(EVENT_FN(Mouse::OnLeave));
}

bool Mouse::IsDown(Button button) const noexcept
{
	if ( m_buttonmap.find(button) == m_buttonmap.end() )
		m_buttonmap[button] = false;
	return m_buttonmap[button];
}

bool Mouse::WasDown(Button button) const noexcept
{
	if ( m_prevButtonmap.find(button) == m_prevButtonmap.end() )
		m_prevButtonmap[button] = false;
	return m_prevButtonmap[button];
}

bool Mouse::IsPressed(Button button) const noexcept
{
	return IsDown(button) && !WasDown(button);
}

bool Mouse::IsReleased(Button button) const noexcept
{
	return !IsDown(button) && WasDown(button);
}

bool Mouse::IsAnyDown() const noexcept
{
	for ( auto &[button, state] : m_buttonmap )
	{
		if ( state )
			return true;
	}
	return false;
}

bool Mouse::IsInScreen() const noexcept
{
	return m_isInScreen;
}

const glm::vec2 &Mouse::GetPosition() const noexcept
{
	return m_position;
}

const glm::vec2 &Mouse::GetDelta() const noexcept
{
	return m_moveDelta;
}

void Mouse::OnPress(const MousePressEvent::Ptr event)
{
	m_buttonmap[event->GetButton()] = true;
}

void Mouse::OnRelease(const MouseReleaseEvent::Ptr event)
{
	m_buttonmap[event->GetButton()] = false;
}

void Mouse::OnMove(const MouseMoveEvent::Ptr event)
{
	m_moveDelta = m_position - event->GetPosition();
	m_position = event->GetPosition();
}

void Mouse::OnEnter(const MouseEnterEvent::Ptr event)
{
	m_moveDelta = m_position - event->GetPosition();
	m_position = event->GetPosition();
}

void Mouse::OnLeave(const MouseLeaveEvent::Ptr event)
{
	m_moveDelta = m_position - event->GetPosition();
	m_position = event->GetPosition();
}

}
