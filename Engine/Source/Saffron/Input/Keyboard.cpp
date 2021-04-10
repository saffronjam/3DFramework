#include "SaffronPCH.h"

#include "Saffron/Input/Keyboard.h"

namespace Se
{
Keyboard* Keyboard::_instance = nullptr;

Keyboard::Keyboard()
{
	SE_CORE_ASSERT(_instance == nullptr, "Keyboard was already instansiated");
	_instance = this;
}

void Keyboard::OnUpdate()
{
	for (auto& [key, state] : _keyboardState)
	{
		_prevKeyboardState[key] = state;
	}
}

void Keyboard::OnEvent(const Event& event)
{
	const auto dispatcher = EventDispatcher(event);

	dispatcher.Try<KeyPressedEvent>(SE_BIND_EVENT_FN(OnKeyPressed));
	dispatcher.Try<KeyReleasedEvent>(SE_BIND_EVENT_FN(OnKeyReleased));
	dispatcher.Try<KeyRepeatedEvent>(SE_BIND_EVENT_FN(OnKeyRepeated));
	dispatcher.Try<TextEvent>(SE_BIND_EVENT_FN(OnText));
}

bool Keyboard::IsPressed(KeyCode keyCode)
{
	return Instance()._keyboardState[keyCode] && !Instance()._prevKeyboardState[keyCode];
}

bool Keyboard::IsReleased(KeyCode keyCode)
{
	return !Instance()._keyboardState[keyCode] && Instance()._prevKeyboardState[keyCode];
}

bool Keyboard::IsDown(KeyCode keyCode)
{
	return Instance()._keyboardState[keyCode];
}

bool Keyboard::IsRepeated(KeyCode keyCode)
{
	return Instance()._keyboardRepeatedState[keyCode];
}

String Keyboard::GetText()
{
	return Instance()._text;
}

void Keyboard::EnableRepeat()
{
	Instance()._repeatEnabled = true;
}

void Keyboard::DisableRepeat()
{
	Instance()._repeatEnabled = false;
}

bool Keyboard::IsRepeatEnabled()
{
	return Instance()._repeatEnabled;
}

bool Keyboard::OnKeyPressed(const KeyPressedEvent& event)
{
	_keyboardState[event.GetKey()] = true;
	return false;
}

bool Keyboard::OnKeyReleased(const KeyReleasedEvent& event)
{
	_keyboardState[event.GetKey()] = false;
	_keyboardRepeatedState[event.GetKey()] = false;
	return false;
}

bool Keyboard::OnKeyRepeated(const KeyRepeatedEvent& event)
{
	_keyboardRepeatedState[event.GetKey()] = true;
	return false;
}

bool Keyboard::OnText(const TextEvent& event)
{
	_text = event.GetCharacter();
	return false;
}
}
