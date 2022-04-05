#include "SaffronPCH.h"

#include "Saffron/Input/Keyboard.h"

#include "Saffron/Common/App.h"

namespace Se
{
Keyboard::Keyboard() :
	Singleton(this)
{
	const auto& win = App::Instance().Window();

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

	return inst._keyStates[key];
}

auto Keyboard::IsKeyPressed(const KeyCode& key) -> bool
{
	auto& inst = Instance();

	return inst._keyStates[key] && !inst._prevKeyStates[key];
}

auto Keyboard::IsKeyReleased(const KeyCode& key) -> bool
{
	auto& inst = Instance();
	return !inst._keyStates[key] && inst._prevKeyStates[key];
}

auto Keyboard::IsAnyKeyDown() -> bool
{
	auto& inst = Instance();
	for (const auto state : inst._keyStates | std::views::values)
	{
		if (state)
		{
			return true;
		}
	}
	return false;
}

auto Keyboard::TextInput() -> std::u32string
{
	auto& inst = Instance();

	return inst._textInputBuffer;
}

void Keyboard::OnKeyPress(const KeyEvent& event)
{
	auto& inst = Instance();

	inst._keyStates[event.Code] = true;
}

void Keyboard::OnKeyRelease(const KeyEvent& event)
{
	auto& inst = Instance();

	inst._keyStates[event.Code] = false;
}

void Keyboard::OnTextInput(const TextEvent& event)
{
	auto& inst = Instance();

	inst._textInputBuffer += event.Unicode;
}
}
