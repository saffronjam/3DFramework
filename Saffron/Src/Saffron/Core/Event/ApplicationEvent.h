#pragma once

#include "Saffron/Core/Event/Event.h"

namespace Se
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