#pragma once


#include "Saffron/Base.h"

namespace Se
{
class Event : public Managed
{
public:
	enum class Type;
	enum Category : unsigned int;

public:
	Event() = default;
	virtual ~Event() = default;

	virtual Type GetType() const = 0;
	virtual const char* GetName() const = 0;
	virtual unsigned int GetCategoryFlags() const = 0;
	virtual String ToString() const;

	bool InCategory(Category category) const;

public:
	mutable bool Consumed = false;

public:
	enum class Type
	{
		None = 0,
		AppTicked,
		AppUpdated,
		AppRendered,
		WindowResized,
		WindowMoved,
		WindowGainedFocus,
		WindowLostFocus,
		WindowClosed,
		WindowDroppedFiles,
		WindowNewTitle,
		WindowNewIcon,
		WindowNewAntiAliasing,
		WindowMinimized,
		WindowMaximized,
		KeyPressed,
		KeyReleased,
		KeyRepeated,
		TextInput,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseWheelScrolled,
		MouseMoved,
		MouseMovedRaw,
		CursorEntered,
		CursorLeft
	};

	enum Category : unsigned int
	{
		CategoryNone = 0,
		CategoryApplication = BIT(0),
		CategoryWindow = BIT(1),
		CategoryInput = BIT(2),
		CategoryKeyboard = BIT(3),
		CategoryMouse = BIT(4)
	};
};

inline std::ostream& operator<<(std::ostream& os, const Event& event)
{
	return os << event.ToString();
}

#define EVENT_CLASS_TYPE(type)	static constexpr Event::Type GetStaticType() { return Event::Type::type; }\
								virtual Event::Type GetType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category) virtual unsigned int GetCategoryFlags() const override { return category; }


class EventDispatcher
{
public:
	explicit EventDispatcher(const Event& event) :
		_event(event)
	{
	}

	template <typename EventType, typename Fn>
	bool Try(const Fn& func) const
	{
		if (!_event.Consumed && typeid(_event) == typeid(EventType))
		{
			_event.Consumed = func(static_cast<const EventType&>(_event));
			return true;
		}
		return false;
	}

private:
	const Event& _event;
};
}
