#include "Saffron/SaffronPCH.h"

#include "Saffron/Input/Keyboard.h"

namespace Se
{

void Keyboard::OnUpdate()
{
	m_prevKeymap = m_keymap;
	for ( auto &[key, value] : m_repeatKeymap )
		value = false;
}

void Keyboard::OnEvent(const Event &event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<KeyboardPressEvent>(SE_EVENT_FN(Keyboard::OnPress));
	dispatcher.Try<KeyboardReleaseEvent>(SE_EVENT_FN(Keyboard::OnRelease));
	dispatcher.Try<KeyboardRepeatEvent>(SE_EVENT_FN(Keyboard::OnRepeat));
}

bool Keyboard::IsDown(Key key) const
{
	if ( m_keymap.find(key) == m_keymap.end() )
		m_keymap[key] = false;
	return m_keymap[key];
}

bool Keyboard::WasDown(Key key) const
{
	if ( m_prevKeymap.find(key) == m_prevKeymap.end() )
		m_prevKeymap[key] = false;
	return m_prevKeymap[key];
}

bool Keyboard::IsPressed(Key key) const
{
	return IsDown(key) && !WasDown(key);
}

bool Keyboard::IsReleased(Key key) const
{
	return !IsDown(key) && WasDown(key);
}

bool Keyboard::IsBeingRepeated(Key key) const
{
	if ( m_repeatKeymap.find(key) == m_repeatKeymap.end() )
		m_repeatKeymap[key] = false;
	return m_repeatKeymap[key];
}

bool Keyboard::IsAnyDown() const
{
	for ( auto &[key, state] : m_keymap )
	{
		if ( state )
			return true;
	}
	return false;
}

void Keyboard::OnPress(const KeyboardPressEvent &event)
{
	m_keymap[event.GetKey()] = true;
}

void Keyboard::OnRelease(const KeyboardReleaseEvent &event)
{
	m_keymap[event.GetKey()] = false;
}

void Keyboard::OnRepeat(const KeyboardRepeatEvent &event)
{
	m_repeatKeymap[event.GetKey()] = true;
}
}
