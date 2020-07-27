#include "Saffron/Input/Keyboard.h"
#include "Saffron/Graphics/Window.h"

namespace Saffron
{

Keyboard::Keyboard(Window &window)
	:
	m_wnd(window)
{
	window.AddEventHandler(this);
}

void Keyboard::Update() noexcept
{
	m_prevKeymap = m_keymap;
	for ( auto &[key, state] : m_repeatKeymap )
		state = false;
}

void Keyboard::OnEvent(const Event::Ptr pEvent)
{
	const EventDispatcher dispatcher(pEvent);
	dispatcher.Try<KeyboardPressEvent>(EVENT_FN(Keyboard::OnPress));
	dispatcher.Try<KeyboardReleaseEvent>(EVENT_FN(Keyboard::OnRelease));
	dispatcher.Try<KeyboardRepeatEvent>(EVENT_FN(Keyboard::OnRepeat));
}

bool Keyboard::IsDown(Key key) const noexcept
{
	if ( m_keymap.find(key) == m_keymap.end() )
		m_keymap[key] = false;
	return m_keymap[key];
}

bool Keyboard::WasDown(Keyboard::Key key) const noexcept
{
	if ( m_prevKeymap.find(key) == m_prevKeymap.end() )
		m_prevKeymap[key] = false;
	return m_prevKeymap[key];
}

bool Keyboard::IsPressed(Key key) const noexcept
{
	return IsDown(key) && !WasDown(key);
}

bool Keyboard::IsReleased(Key key) const noexcept
{
	return !IsDown(key) && WasDown(key);
}

bool Keyboard::IsBeingRepeated(Key key) const noexcept
{
	if ( m_repeatKeymap.find(key) == m_repeatKeymap.end() )
		m_repeatKeymap[key] = false;
	return m_repeatKeymap[key];
}

bool Keyboard::IsAnyDown() const noexcept
{
	for ( auto &[key, state] : m_keymap )
	{
		if ( state )
			return true;
	}
	return false;
}

void Keyboard::OnPress(const KeyboardPressEvent::Ptr event)
{
	m_keymap[event->GetKey()] = true;
}

void Keyboard::OnRelease(const KeyboardReleaseEvent::Ptr event)
{
	m_keymap[event->GetKey()] = false;
}

void Keyboard::OnRepeat(const KeyboardRepeatEvent::Ptr event)
{
	m_repeatKeymap[event->GetKey()] = true;
}
}