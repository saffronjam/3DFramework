#pragma once

#include "Saffron/Core/Event.h"
#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Input/MouseCodes.h"

namespace Se
{

class MousePressEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MousePress);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MousePressEvent(ButtonCode button) : m_Button(button) {}

	ButtonCode GetButton() const { return m_Button; }
	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << GetName() << " Button: " << m_Button;
		return oss.str();
	}

private:
	ButtonCode m_Button;
};

class MouseReleaseEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseRelease);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	explicit MouseReleaseEvent(ButtonCode button) : m_Button(button) {}

	ButtonCode GetButton() const { return m_Button; }
	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << GetName() << " Button: " << m_Button;
		return oss.str();
	}

private:
	ButtonCode m_Button;
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

	std::string ToString() const override
	{
		std::stringstream ss;
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
	explicit MouseMoveEvent(const glm::vec2 &position) : m_Position(position) {}

	const glm::vec2 &GetPosition() const { return m_Position; }
	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << GetName() << " Position: " << m_Position.x << ", " << m_Position.y;
		return oss.str();
	}

private:
	glm::vec2 m_Position;
};

class MouseEnterEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseEnter);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	std::string ToString() const override { return GetName(); }
};

class MouseLeaveEvent : public Event
{
public:
	EVENT_CLASS_TYPE(MouseLeave);
	EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput);

public:
	std::string ToString() const override { return GetName(); }

};

}
