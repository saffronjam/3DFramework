#pragma once

#include "Saffron/Event/Event.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class WindowResizeEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowResize);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	WindowResizeEvent(int width, int height) : m_Width(width), m_Height(height) {}

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << GetName() << " " << m_Width << "x" << m_Height;
		return oss.str();
	}

private:
	int m_Width, m_Height;
};

class WindowMoveEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowMove);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	explicit WindowMoveEvent(const glm::vec2 &position) : m_Position(position) {}

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

class WindowGainFocusEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowGainFocus);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	std::string ToString() const override { return GetName(); }

};

class WindowLostFocusEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowLostFocus);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	std::string ToString() const override { return GetName(); }

};

class WindowCloseEvent : public Event
{
public:
	EVENT_CLASS_TYPE(WindowClose);
	EVENT_CLASS_CATEGORY(CategoryWindow);

public:
	std::string ToString() const override { return GetName(); }

};
}