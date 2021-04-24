#include "SaffronPCH.h"

#include "Saffron/Core/App.h"
#include "Saffron/Input/Keyboard.h"

namespace Se
{
Keyboard* Keyboard::_instance = nullptr;

EventSubscriberList<const KeyPressedEvent&> Keyboard::Pressed;
EventSubscriberList<const KeyReleasedEvent&> Keyboard::Released;
EventSubscriberList<const KeyRepeatedEvent&> Keyboard::Repeated;
EventSubscriberList<const TextInputEvent&> Keyboard::TextInput;

Keyboard::Keyboard()
{
	Debug::Assert(_instance == nullptr, "Keyboard was already instansiated");;
	_instance = this;

	const auto& window = App::Instance().GetWindow();
	window.KeyPressed += SE_BIND_EVENT_FN(OnKeyPressed);
	window.KeyReleased += SE_BIND_EVENT_FN(OnKeyReleased);
	window.KeyRepeated += SE_BIND_EVENT_FN(OnKeyRepeated);
	window.TextInput += SE_BIND_EVENT_FN(OnTextInput);
}

void Keyboard::OnUpdate()
{
	for (auto& [key, state] : _keyboardState)
	{
		_prevKeyboardState[key] = state;
	}
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
	Pressed.Invoke(event);
	_keyboardState[event.GetKey()] = true;
	return false;
}

bool Keyboard::OnKeyReleased(const KeyReleasedEvent& event)
{
	Released.Invoke(event);
	_keyboardState[event.GetKey()] = false;
	_keyboardRepeatedState[event.GetKey()] = false;
	return false;
}

bool Keyboard::OnKeyRepeated(const KeyRepeatedEvent& event)
{
	Repeated.Invoke(event);
	_keyboardRepeatedState[event.GetKey()] = true;
	return false;
}

bool Keyboard::OnTextInput(const TextInputEvent& event)
{
	TextInput.Invoke(event);
	_text = event.GetCharacter();
	return false;
}
}
