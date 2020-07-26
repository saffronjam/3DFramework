#include "Mouse.h"

#include <GLFW/glfw3.h>

#include "Window.h"

Mouse::Mouse(Window &window)
        : m_isInScreen(true),
          m_wnd(window),
          m_moveDelta(0.0f, 0.0f)
{
    window.AddEventHandler(Event::Type::Mouse, this);
}

void Mouse::Update() noexcept
{
    m_prevButtonmap = m_buttonmap;
    m_moveDelta = glm::vec2{0.0f, 0.0f};
}

void Mouse::HandleEvent(const Event &event)
{
    switch (event.mouse.action)
    {
    case Action::Press:
        OnPress(event.mouse);
        break;
    case Action::Release:
        OnRelease(event.mouse);
        break;
    case Action::Move:
        OnMove(event.mouse);
        break;
    case Action::Enter:
        OnEnter(event.mouse);
        break;
    case Action::Leave:
        OnLeave(event.mouse);
        break;
    default:
        break;
    }
}

void Mouse::AddCallback(Mouse::Action action, const Mouse::Callback &callback)
{
    m_callbacks[action].push_back(callback);
}

bool Mouse::IsDown(Button button) const noexcept
{
    if (m_buttonmap.find(button) == m_buttonmap.end())
        m_buttonmap[button] = false;
    return m_buttonmap[button];
}

bool Mouse::WasDown(Button button) const noexcept
{
    if (m_prevButtonmap.find(button) == m_prevButtonmap.end())
        m_prevButtonmap[button] = false;
    return m_prevButtonmap[button];
}

bool Mouse::IsPressed(Button button) const noexcept
{
    return IsDown(button) && !WasDown(button);
}

bool Mouse::IsReleased(Button button) const noexcept
{
    return !IsDown(button) && WasDown(button);
}

bool Mouse::IsAnyDown() const noexcept
{
    for (auto&[button, state] : m_buttonmap)
    {
        if (state)
            return true;
    }
    return false;
}

bool Mouse::IsInScreen() const noexcept
{
    return m_isInScreen;
}

const glm::vec2 &Mouse::GetPosition() const noexcept
{
    return m_position;
}

const glm::vec2 &Mouse::GetDelta() const noexcept
{
    return m_moveDelta;
}

void Mouse::OnPress(const MouseEvent &event)
{
    m_buttonmap[event.button] = true;
    for (auto &callback : m_callbacks[Action::Press])
        callback(event);
}

void Mouse::OnRelease(const MouseEvent &event)
{
    m_buttonmap[event.button] = false;
    for (auto &callback : m_callbacks[Action::Release])
        callback(event);
}
void Mouse::OnMove(const MouseEvent &event)
{
    m_moveDelta = m_position - event.position;
    m_position = event.position;
    for (auto &callback : m_callbacks[Action::Move])
        callback(event);
}

void Mouse::OnEnter(const struct MouseEvent &event)
{
    for (auto &callback : m_callbacks[Action::Enter])
        callback(event);
}

void Mouse::OnLeave(const struct MouseEvent &event)
{
    for (auto &callback : m_callbacks[Action::Leave])
        callback(event);
}

