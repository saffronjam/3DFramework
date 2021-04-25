#pragma once

#include "Saffron/Core/Event.h"
#include "Saffron/Input/InputCodes.h"

namespace Se
{
class KeyPressedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(KeyPressed);

	EVENT_CLASS_CATEGORY(CategoryKeyboard | CategoryInput);

public:
	explicit KeyPressedEvent(KeyCode key) :
		_key(key)
	{
	}

	KeyCode GetKey() const { return _key; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Key: " << static_cast<int>(_key);
		return oss.str();
	}

private:
	KeyCode _key;
};

class KeyReleasedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(KeyReleased);

	EVENT_CLASS_CATEGORY(CategoryKeyboard | CategoryInput);

public:
	explicit KeyReleasedEvent(KeyCode key) :
		_key(key)
	{
	}

	KeyCode GetKey() const { return _key; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Key: " << static_cast<int>(_key);
		return oss.str();
	}

private:
	KeyCode _key;
};

class KeyRepeatedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(KeyRepeated);

	EVENT_CLASS_CATEGORY(CategoryKeyboard | CategoryInput);

public:
	explicit KeyRepeatedEvent(KeyCode key) :
		_key(key)
	{
	}

	KeyCode GetKey() const { return _key; }

	String ToString() const override
	{
		OStringStream oss;
		oss << GetName() << " Key: " << static_cast<int>(_key);
		return oss.str();
	}

private:
	KeyCode _key;
};

class TextInputEvent : public Event
{
public:
	EVENT_CLASS_TYPE(TextInput);

	EVENT_CLASS_CATEGORY(CategoryKeyboard | CategoryInput);

public:
	explicit TextInputEvent(uchar character) :
		_character(character)
	{
	}

	uchar GetCharacter() const { return _character; }

	String ToString() const override
	{
		OStringStream ss;
		ss << GetName() << static_cast<int>(_character);
		return ss.str();
	}

private:
	uchar _character;
};
}
