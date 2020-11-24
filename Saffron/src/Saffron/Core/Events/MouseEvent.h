#pragma once

#include "Saffron/Core/Event.h"
#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Input/MouseButtonCodes.h"

namespace Se
{

class MousePressEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MousePress);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MousePressEvent(MouseButtonCode button) : m_Button(button) {}

	MouseButtonCode GetButton() const { return m_Button; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Button: " << m_Button;
		return oss.str();
	}

private:
	MouseButtonCode m_Button;
};

class MouseReleaseEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseRelease);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MouseReleaseEvent(MouseButtonCode button) : m_Button(button) {}

	MouseButtonCode GetButton() const { return m_Button; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Button: " << m_Button;
		return oss.str();
	}

private:
	MouseButtonCode m_Button;
};

class MouseScrollEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseScroll);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput)

public:
	MouseScrollEvent(float xOffset, float yOffset) : m_OffsetX(xOffset), m_OffsetY(yOffset) {}

	float GetOffsetX() const { return m_OffsetX; }
	float GetOffsetY() const { return m_OffsetY; }

	String ToString() const override
	{
		StringStream ss;
		ss << GetName() << " Offsets: " << GetOffsetX() << ", " << GetOffsetY();
		return ss.str();
	}

private:
	float m_OffsetX, m_OffsetY;
};

class MouseMoveEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseMove);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MouseMoveEvent(const Vector2f &position) : m_Position(position) {}

	const Vector2f &GetPosition() const { return m_Position; }
	String ToString() const override
	{
		OutputStringStream oss;
		oss << GetName() << " Position: " << m_Position.x << ", " << m_Position.y;
		return oss.str();
	}

private:
	Vector2f m_Position;
};

class MouseEnterEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseEnter);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	String ToString() const override { return GetName(); }
};

class MouseLeaveEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseLeave);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	String ToString() const override { return GetName(); }

};

}
