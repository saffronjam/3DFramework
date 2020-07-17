#include "Mouse.h"

void Mouse::Update() noexcept
{
    m_buttonmap = m_prevButtonmap;
}

void Mouse::HandleEvent(const Event &event)
{
    switch (static_cast<Mouse::Action>(event))
    {
    case Mouse::Press:
        pWnd->mouse.OnPress(static_cast<Mouse::Button>(button));
        break;
    case Mouse::Release:
        pWnd->mouse.OnRelease(static_cast<Mouse::Button>(button));
        break;
        break;
    default: // Keyboard::Action::Unknown not handled
        break;
    }
}

bool Mouse::IsDown(Button button)
{
    return m_buttonmap[button];
}

bool Mouse::IsPressed(Button button)
{
    return m_buttonmap[button] && !m_prevButtonmap[button];
}

bool Mouse::IsReleased(Button button)
{
    return !m_buttonmap[button] && m_prevButtonmap[button];
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
    m_buttonmap[button] = true;
}


