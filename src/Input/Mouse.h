#pragma once

#include <map>
#include <functional>
#include <glm/vec2.hpp>

#include "IEventHandler.h"

class Mouse : public IEventHandler
{
    friend class Window;

public:
    enum Button
    {
        Num1 = 0,
        Num2 = 1,
        Num3 = 2,
        Num4 = 3,
        Num5 = 4,
        Num6 = 5,
        Num7 = 6,
        Num8 = 7,
        Last = Num8,
        Left = Num1,
        Right = Num2,
        Middle = Num3
    };
    enum Action
    {
        Release = 0,
        Press = 1
    };
    using Callback = std::function<void(const struct MouseEvent &event)>;

public:
    explicit Mouse(class Window &window);

    void Update() noexcept;
    void HandleEvent(const Event &event) override;

    void AddCallback(Action action, const Callback &callback) noexcept
    {
        switch (action)
        {
        case Action::Press:
        case Action::Release:
            m_callbacks[action].push_back(callback);
            break;
        default:
            break;
        }
    }

    bool IsDown(Button button);
    bool WasDown(Button button);
    bool IsPressed(Button button);
    bool IsReleased(Button button);
    bool IsAnyDown();

private:
    void OnPress(const struct MouseEvent &event);
    void OnRelease(const struct MouseEvent &event);

private:
    std::map<Button, bool> m_buttonmap;
    std::map<Button, bool> m_prevButtonmap;
    std::map<Action, std::vector<Callback>> m_callbacks;
};