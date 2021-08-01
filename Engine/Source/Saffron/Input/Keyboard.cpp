#include "SaffronPCH.h"

#include "Saffron/Input/Keyboard.h"

#include "Saffron/Common/App.h"

namespace Se
{
Keyboard::Keyboard() :
	SingleTon(this)
{
	for (int i = 0; i < _keyStates.size(); i++)
	{
		_keyStates[i] = false;
		_prevKeyStates[i] = false;
	}
	
	auto& win = App::Instance().Window();

	win.KeyPressed += SE_EV_ACTION(Keyboard::OnKeyPress);
	win.KeyReleased += SE_EV_ACTION(Keyboard::OnKeyRelease);
	win.TextEntered += SE_EV_ACTION(Keyboard::OnTextInput);
}

void Keyboard::OnUpdate()
{
	auto& inst = Instance();
	inst._prevKeyStates = inst._keyStates;
	inst._textInputBuffer.clear();
}

auto Keyboard::IsKeyDown(const KeyCode& key) -> bool
{
	auto& inst = Instance();

	return inst._keyStates[static_cast<int>(key)];
}

auto Keyboard::IsKeyPressed(const KeyCode& key) -> bool
{
	auto& inst = Instance();

	return inst._keyStates[static_cast<int>(key)] && !inst._prevKeyStates[static_cast<int>(key)];
}

auto Keyboard::IsKeyReleased(const KeyCode& key) -> bool
{
	auto& inst = Instance();
	return !inst._keyStates[static_cast<int>(key)] && inst._prevKeyStates[static_cast<int>(key)];
}

auto Keyboard::IsAnyKeyDown() -> bool
{
	auto& inst = Instance();
	return std::ranges::any_of(inst._keyStates, [](bool element) { return element; });
}

auto Keyboard::TextInput() -> std::u32string
{
	auto& inst = Instance();

	return inst._textInputBuffer;
}

void Keyboard::OnKeyPress(const KeyEvent& event)
{
	auto& inst = Instance();

	inst._keyStates[static_cast<int>(event.Code)] = true;
}

void Keyboard::OnKeyRelease(const KeyEvent& event)
{
	auto& inst = Instance();

	inst._keyStates[static_cast<int>(event.Code)] = false;
}

void Keyboard::OnTextInput(const TextEvent& event)
{
	auto& inst = Instance();

	inst._textInputBuffer += event.Unicode;
}
}
