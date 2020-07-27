#pragma once

#include "Saffron/Event/Event.h"
#include "Saffron/Input/KeyCodes.h"

namespace Se
{
class KeyboardPressEvent : public Event
{
public:
	EVENT_CLASS_TYPE(KeyboardPress);
	EVENT_CLASS_CATEGORY(CategoryKeyboard | CategoryInput);

public:
	explicit KeyboardPressEvent(KeyCode key) : m_Key(key) {}

	KeyCode GetKey() const { return m_Key; }
	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << GetName() << " Key: " << m_Key;
		return oss.str();
	}

private:
	KeyCode m_Key;
};

class KeyboardReleaseEvent : public Event
{
public:
	EVENT_CLASS_TYPE(KeyboardRelease);
	EVENT_CLASS_CATEGORY(CategoryKeyboard | CategoryInput);

public:
	explicit KeyboardReleaseEvent(KeyCode key) : m_Key(key) {}

	KeyCode GetKey() const { return m_Key; }
	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << GetName() << " Key: " << m_Key;
		return oss.str();
	}

private:
	KeyCode m_Key;
};

class KeyboardRepeatEvent : public Event
{
public:
	EVENT_CLASS_TYPE(KeyboardRepeat);
	EVENT_CLASS_CATEGORY(CategoryKeyboard | CategoryInput);

public:
	explicit KeyboardRepeatEvent(KeyCode key) : m_Key(key) {}

	KeyCode GetKey() const { return m_Key; }
	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << GetName() << " Key: " << m_Key;
		return oss.str();
	}

private:
	KeyCode m_Key;
};

}