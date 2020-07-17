#pragma once

#include "Keyboard.h"
#include "Mouse.h"

struct KeyboardEvent
{
    Keyboard::Action action;
    Keyboard::Key key;
};

struct MouseEvent
{
    Mouse::Action action;
    Mouse::Button button;
};

enum class EventType
{
    Keyboard,
    Mouse,
    Unknown
};

union Event
{
    EventType type = EventType::Unknown;
    MouseEvent mouse;
    KeyboardEvent keyboard;
};

