#pragma once

union Event;

class IEventHandler
{
public:
    virtual void HandleEvent(const Event& event) = 0;
};

