#pragma once

#include "Keyboard.h"
#include "Mouse.h"

class Event
{
public:
    enum class Type
    {
        Keyboard,
        Mouse,
        Unknown
    };

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

    Type type;
    union
    {
        KeyboardEvent keyboard;
        MouseEvent mouse;
    };
};

