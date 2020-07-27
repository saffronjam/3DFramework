#pragma once

#include "Saffron/Event/Event.h"

namespace Se
{
class EventHandler
{
public:
	virtual void OnEvent(const Event::Ptr &pEvent) = 0;
};
}