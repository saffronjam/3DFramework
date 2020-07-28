#pragma once

#include "Saffron/Event/Event.h"

namespace Se
{
class EventHandler
{
public:
	virtual void OnEvent(const Event &event) = 0;
};
}