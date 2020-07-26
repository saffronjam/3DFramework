#pragma once

#include <map>
#include <functional>
#include <optional>

#include "IEventHandler.h"
#include "SeMath.h"

class Mouse : public IEventHandler
{
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
        Middle = Num3,
        Undefined = 100
    };
    enum Action
    {
        Release = 0,
        Press = 1,
        Move = 2,
        Enter = 3,
        Leave = 4
    };
    using Callback = std::function<void(const struct MouseEvent &event)>;

public:
    explicit Mouse(class Window &window);

    void Update() noexcept;
    void HandleEvent(const Event &event) override;

    void AddCallback(Action action, const Callback &callback);

    bool IsDown(Button button) const noexcept;
    bool WasDown(Button button) const noexcept;
    bool IsPressed(Button button) const noexcept;
    bool IsReleased(Button button) const noexcept;
    bool IsAnyDown() const noexcept;
    bool IsInScreen() const noexcept;

    [[nodiscard]] const glm::vec2 &GetPosition() const noexcept;
    [[nodiscard]] const glm::vec2 &GetDelta() const noexcept;

private:
    void OnPress(const struct MouseEvent &event);
    void OnRelease(const struct MouseEvent &event);
    void OnMove(const struct MouseEvent &event);
    void OnEnter(const struct MouseEvent &event);
    void OnLeave(const struct MouseEvent &event);

private:
    class Window &m_wnd;

    mutable std::map<Button, bool> m_buttonmap;
    mutable std::map<Button, bool> m_prevButtonmap;
    std::map<Action, std::vector<Callback>> m_callbacks;

    glm::vec2 m_position;
    glm::vec2 m_moveDelta;

    bool m_isInScreen;
};