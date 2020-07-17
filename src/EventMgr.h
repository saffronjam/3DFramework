#pragma once

#include <map>

#include "Window.h"
#include "IEventHandler.h"

class EventMgr
{
public:
    static void Push(Window *window, const Event &event);
    static void PollAll(Window *window);
    static void AddHandler(Window *window, EventType type, IEventHandler *handler);

private:
    static std::map<Window *, std::vector<Event>> m_eventContainerMap;
    static std::map<Window *, std::multimap<EventType, IEventHandler *>> m_handlers;
};

