#pragma once

#include "Saffron/Common/Event.h"

namespace Se
{
class AppTickEvent : public Event
{
public:
	EVENT_CLASS_TYPE(AppTick);

	EVENT_CLASS_CATEGORY(CategoryApplication);

public:
	AppTickEvent() = default;
};

class AppUpdateEvent : public Event
{
public:
	EVENT_CLASS_TYPE(AppUpdate);

	EVENT_CLASS_CATEGORY(CategoryApplication);

public:
	AppUpdateEvent() = default;
};

class AppRenderEvent : public Event
{
public:
	EVENT_CLASS_TYPE(AppRender);

	EVENT_CLASS_CATEGORY(CategoryApplication);

public:
	AppRenderEvent() = default;
};
}
