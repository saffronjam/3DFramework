#include "Keyboard.h"

void ve::Keyboard::Update()
{
	for ( int i = 0; i < m_numKeys; i++ )
		m_prevKeyStates[i] = m_keyStates[i];
}

bool ve::Keyboard::KeyIsDown(uint8_t keycode) const noexcept
{
	return m_keyStates[keycode];
}

bool ve::Keyboard::KeyIsPressed(uint8_t keycode) const noexcept
{
	return m_keyStates[keycode] && !m_prevKeyStates[keycode];
}

bool ve::Keyboard::KeyIsReleased(uint8_t keycode) const noexcept
{
	return !m_keyStates[keycode] && m_prevKeyStates[keycode];
}

std::optional<ve::Keyboard::Event> ve::Keyboard::ReadKey() noexcept
{
	if ( m_keyBuffer.size() > 0u )
	{
		Keyboard::Event e = m_keyBuffer.front();
		m_keyBuffer.pop();
		return e;
	}
	else
	{
		return std::nullopt;
	}
}

bool ve::Keyboard::KeyIsEmpty() const noexcept
{
	return m_keyBuffer.empty();
}

void ve::Keyboard::ClearKey() noexcept
{
	m_keyBuffer = std::queue<Event>();
}

std::optional<char> ve::Keyboard::ReadChar() noexcept
{
	if ( m_charBuffer.size() > 0u )
	{
		uint8_t c = m_charBuffer.front();
		m_charBuffer.pop();
		return c;
	}
	else
	{
		return std::nullopt;
	}
}

bool ve::Keyboard::CharIsEmpty() const noexcept
{
	return false;
}

void ve::Keyboard::ClearChar() noexcept
{
	m_charBuffer = std::queue<char>();
}

void ve::Keyboard::Clear() noexcept
{
	ClearKey();
	ClearChar();
}

void ve::Keyboard::EnableAutoRepeat() noexcept
{
	m_autorepeat = true;
}

void ve::Keyboard::DisableAutoRepeat() noexcept
{
	m_autorepeat = false;

}

bool ve::Keyboard::AutoRepeatIsOn() const noexcept
{
	return m_autorepeat;
}

void ve::Keyboard::OnKeyPress(uint8_t keycode) noexcept
{
	m_keyStates[keycode] = true;
	m_keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press));
	TrimBuffer(m_keyBuffer);
}

void ve::Keyboard::OnKeyRelease(uint8_t keycode) noexcept
{
	m_keyStates[keycode] = false;
	m_keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release));
	TrimBuffer(m_keyBuffer);
}

void ve::Keyboard::OnChar(char character) noexcept
{
	m_charBuffer.push(character);
	TrimBuffer(m_charBuffer);
}

void ve::Keyboard::ClearState() noexcept
{
	m_keyStates.reset();
	m_prevKeyStates.reset();
}

template <typename T>
void ve::Keyboard::TrimBuffer(std::queue<T> &buffer) noexcept
{
	while ( buffer.size() > m_bufferSize )
		buffer.pop();
}