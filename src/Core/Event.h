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
    glm::vec2 position;
};

class Event
{
public:
    enum class Type
    {
        Keyboard,
        Mouse,
        Unknown [[maybe_unused]]
    };

    Type type;
    union
    {
        KeyboardEvent keyboard;
        MouseEvent mouse;
    };
};

