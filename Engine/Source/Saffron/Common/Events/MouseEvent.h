#pragma once

#include "Saffron/Common/Event.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Input/InputCodes.h"

namespace Se
{
class MouseButtonPressedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseButtonPressed);

	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MouseButtonPressedEvent(MouseButtonCode button) :
		_button(button)
	{
	}

	MouseButtonCode GetButton() const { return _button; }

	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Button: " << static_cast<int>(_button);
		return oss.str();
	}

private:
	MouseButtonCode _button;
};

class MouseButtonReleasedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseButtonReleased);

	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MouseButtonReleasedEvent(MouseButtonCode button) :
		_button(button)
	{
	}

	MouseButtonCode GetButton() const { return _button; }

	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Button: " << static_cast<int>(_button);
		return oss.str();
	}

private:
	MouseButtonCode _button;
};

class MouseWheelScrolledEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseWheelScrolled);

	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput)

public:
	MouseWheelScrolledEvent(MouseWheelCode wheel, float offset) :
		_wheel(wheel),
		_offset(offset)
	{
	}

	MouseWheelCode GetWheel() const { return _wheel; }

	float GetOffset() const { return _offset; }

	String ToString() const override
	{
		StringStream ss;
		ss << GetName() << " Offset: " << _offset;
		return ss.str();
	}

private:
	MouseWheelCode _wheel;
	float _offset;
};

class MouseMovedEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseMoved);

	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MouseMovedEvent(const Vector2& position) :
		_position(position)
	{
	}

	const Vector2& GetPosition() const { return _position; }

	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Position: " << _position.x << ", " << _position.y;
		return oss.str();
	}

private:
	Vector2 _position;
};

class MouseMovedRawEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseMovedRaw);

	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MouseMovedRawEvent(const Vector2& delta) :
		_delta(delta)
	{
	}

	const Vector2& GetDelta() const { return _delta; }

	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Position: " << _delta.x << ", " << _delta.y;
		return oss.str();
	}

private:
	Vector2 _delta;
};

class CursorEnteredEvent : public Event
{
public:
	EVENT_CLASS_TYPE(CursorEntered);

	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	String ToString() const override { return GetName(); }
};

class CursorLeftEvent : public Event
{
public:
	EVENT_CLASS_TYPE(CursorLeft);

	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	String ToString() const override { return GetName(); }
};
}
