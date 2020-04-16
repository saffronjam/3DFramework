#pragma once

#include <queue>
#include <bitset>
#include <optional>

namespace ve
{
class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type m_type;
		uint8_t m_code;
	public:
		Event(Type type = Type::Invalid, uint8_t code = 0u) noexcept
			:
			m_type(type),
			m_code(code)
		{
		}
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard &) = delete;
	Keyboard &operator=(const Keyboard &) = delete;

	void Update();

	bool KeyIsDown(uint8_t keycode) const noexcept;
	bool KeyIsPressed(uint8_t keycode) const noexcept;
	bool KeyIsReleased(uint8_t keycode) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void ClearKey() noexcept;

	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void ClearChar() noexcept;

	void Clear() noexcept;

	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool AutoRepeatIsOn() const noexcept;

private:
	void OnKeyPress(uint8_t keycode) noexcept;
	void OnKeyRelease(uint8_t keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template <typename T>
	static void TrimBuffer(std::queue<T> &buffer) noexcept;
private:
	static constexpr uint32_t m_numKeys = 256u;
	static constexpr uint32_t m_bufferSize = 16u;
	bool m_autorepeat = false;
	std::bitset<m_numKeys> m_keyStates;
	std::bitset<m_numKeys> m_prevKeyStates;
	std::queue<Event> m_keyBuffer;
	std::queue<char> m_charBuffer;
};
}

