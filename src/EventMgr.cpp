#include "EventMgr.h"

void EventMgr::Push(Window *window, const Event &event)
{
    m_eventContainerMap.emplace(std::make_pair(window, std::vector<Event>()));
}

void EventMgr::PollAll(Window *window)
{
    for (auto &[window, eventContainer] : m_eventContainerMap)
    {
        for (auto &event : eventContainer)
        {
            if (m_handlers[window].find(event.type) != m_handlers[window].end())
            {
                auto range = m_handlers[window].equal_range(event.type);
                for(auto it = range.first; it != range.second; it++)
                {
                    it->second->HandleEvent(event);
                }
            }
        }
    }
}
void EventMgr::AddHandler(Window *window, EventType type, IEventHandler *handler)
{
    m_handlers[window].emplace(std::make_pair(type, handler));
}
