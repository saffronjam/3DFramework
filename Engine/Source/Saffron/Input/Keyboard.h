#pragma once

#include <array>

#include "Saffron/Base.h"
#include "Saffron/Event/Event.h"

namespace Se
{
class Keyboard : public Singleton<Keyboard>
{
public:
	Keyboard();

	void OnUpdate();

	static auto IsKeyDown(const KeyCode& key) -> bool;
	static auto IsKeyPressed(const KeyCode& key) -> bool;
	static auto IsKeyReleased(const KeyCode& key) -> bool;
	static auto IsAnyKeyDown() -> bool;

	static auto TextInput() -> std::u32string;

private:
	static void OnKeyPress(const KeyEvent& event);
	static void OnKeyRelease(const KeyEvent& event);
	static void OnTextInput(const TextEvent& event);

private:
	std::unordered_map<KeyCode, bool> _keyStates{};
	std::unordered_map<KeyCode, bool> _prevKeyStates{};
	std::u32string _textInputBuffer;
};
}
