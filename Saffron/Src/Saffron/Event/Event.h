#pragma once

#include "Saffron/SaffronPCH.h"
#include "Saffron/Config.h"
#include "Saffron/System/Macros.h"

namespace Se
{

class Event
{
public:
	enum class Type;
	enum Category : unsigned int;

	using Ptr = std::shared_ptr<Event>;

public:
	Event() = default;
	virtual ~Event() = default;

	virtual Type GetType() const = 0;
	virtual const char *GetName() const = 0;
	virtual unsigned int GetCategoryFlags() const = 0;
	virtual std::string ToString() const;

	bool InCategory(Category category) const;

public:
	enum class Type
	{
		None = 0,
		Application,
		WindowResize, WindowMove, WindowGainFocus, WindowLostFocus, WindowClose,
		KeyboardPress, KeyboardRelease, KeyboardRepeat,
		MousePress, MouseRelease, MouseMove, MouseEnter, MouseLeave
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

inline std::ostream &operator<<(std::ostream &os, const Event &e)
{
	return os << e.ToString();
}

#define EVENT_CLASS_TYPE(type)  using Ptr = std::shared_ptr<type##Event>;\
								static constexpr Event::Type GetStaticType() { return Event::Type::type; }\
								virtual Event::Type GetType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }\

#define EVENT_CLASS_CATEGORY(category) virtual unsigned int GetCategoryFlags() const override { return category; }


class EventDispatcher
{
public:
	explicit EventDispatcher(const Event::Ptr &pEvent) : m_pEvent(pEvent) {}

	template<typename EventType, typename Fn>
	void Try(const Fn &func) const
	{
		if ( typeid(*m_pEvent) == typeid(EventType) )
			func(std::static_pointer_cast<EventType>(m_pEvent));
	}

private:
	const Event::Ptr &m_pEvent;
};

}
