#pragma once

class Event;

class IEventHandler
{
public:
    virtual void HandleEvent(const Event &event) = 0;
};