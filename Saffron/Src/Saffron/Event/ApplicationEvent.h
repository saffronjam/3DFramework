#pragma once

#include "Saffron/Event/Event.h"

namespace Saffron
{
class ApplicationEvent : public Event
{
public:
	EVENT_CLASS_TYPE(Application);
	EVENT_CLASS_CATEGORY(CategoryApplication);

public:
	std::string ToString() const override { return GetName(); }
};

}