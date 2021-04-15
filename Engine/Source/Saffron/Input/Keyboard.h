#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Events/KeyboardEvent.h"
#include "Saffron/Input/InputCodes.h"

namespace Se
{
class Keyboard : public MemManaged<Keyboard>
{
public:
	Keyboard();

	void OnUpdate();

	static bool IsPressed(KeyCode keyCode);
	static bool IsReleased(KeyCode keyCode);
	static bool IsDown(KeyCode keyCode);
	static bool IsRepeated(KeyCode keyCode);

	static String GetText();

	static void EnableRepeat();
	static void DisableRepeat();
	static bool IsRepeatEnabled();

private:
	bool OnKeyPressed(const KeyPressedEvent& event);
	bool OnKeyReleased(const KeyReleasedEvent& event);
	bool OnKeyRepeated(const KeyRepeatedEvent& event);
	bool OnTextInput(const TextInputEvent& event);

	static Keyboard& Instance()
	{
		SE_CORE_ASSERT(_instance != nullptr, "Keyboard was not instansiated")
		return *_instance;
	}

public:
	static EventSubscriberList<const KeyPressedEvent&> Pressed;
	static EventSubscriberList<const KeyReleasedEvent&> Released;
	static EventSubscriberList<const KeyRepeatedEvent&> Repeated;
	static EventSubscriberList<const TextInputEvent&> TextInput;

private:
	UnorderedMap<KeyCode, bool> _keyboardState;
	UnorderedMap<KeyCode, bool> _prevKeyboardState;
	UnorderedMap<KeyCode, bool> _keyboardRepeatedState;
	String _text;

	bool _repeatEnabled = true;

	static Keyboard* _instance;
};
}
