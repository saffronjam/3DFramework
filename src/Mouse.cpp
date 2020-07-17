#include "Mouse.h"

#include "Window.h"

Mouse::Mouse(Window &window)
{
    window.AddEventHandler(Event::Type::Mouse, this);
}

void Mouse::Update() noexcept
{
    m_prevButtonmap = m_buttonmap;
}

void Mouse::HandleEvent(const Event &event)
{
    switch (event.mouse.action)
    {
    case Mouse::Press:
        OnPress(event.mouse.button);
        break;
    case Mouse::Release:
        OnRelease(event.mouse.button);
        break;
    default:
        break;
    }
}

bool Mouse::IsDown(Button button)
{
    if (m_buttonmap.find(button) == m_buttonmap.end())
        m_buttonmap[button] = false;
    return m_buttonmap[button];
}

bool Mouse::WasDown(Button button)
{
    if (m_prevButtonmap.find(button) == m_prevButtonmap.end())
        m_prevButtonmap[button] = false;
    return m_prevButtonmap[button];
}

bool Mouse::IsPressed(Button button)
{
    return IsDown(button) && !WasDown(button);
}

bool Mouse::IsReleased(Button button)
{
    return !IsDown(button) && WasDown(button);
}

bool Mouse::IsAnyDown()
{
    for (auto&[button, state] : m_buttonmap)
    {
        if (state)
            return true;
    }
    return false;
}

void Mouse::OnPress(Button button) noexcept
{
    m_buttonmap[button] = true;
}

void Mouse::OnRelease(Button button) noexcept
{
    m_buttonmap[button] = false;
}

