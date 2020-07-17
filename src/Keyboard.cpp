#include "Keyboard.h"

#include "Window.h"

Keyboard::Keyboard(Window &window)
{
    window.AddEventHandler(Event::Type::Keyboard, this);
}

void Keyboard::Update() noexcept
{
    m_prevKeymap = m_keymap;
    for (auto&[key, state] : m_repeatKeymap)
        state = false;
}

void Keyboard::HandleEvent(const Event &event)
{
    switch (event.keyboard.action)
    {
    case Keyboard::Press:
        OnPress(event.keyboard.key);
        break;
    case Keyboard::Release:
        OnRelease(event.keyboard.key);
        break;
    case Keyboard::Repeat:
        OnRepeat(event.keyboard.key);
        break;
    default:
        break;
    }
}

bool Keyboard::IsDown(Key key)
{
    if (m_keymap.find(key) == m_keymap.end())
        m_keymap[key] = false;
    return m_keymap[key];
}

bool Keyboard::WasDown(Keyboard::Key key)
{
    if (m_prevKeymap.find(key) == m_prevKeymap.end())
        m_prevKeymap[key] = false;
    return m_prevKeymap[key];
}

bool Keyboard::IsPressed(Key key)
{
    return IsDown(key) && !WasDown(key);
}

bool Keyboard::IsReleased(Key key)
{
    return !IsDown(key) && WasDown(key);
}

bool Keyboard::IsBeingRepeated(Key key)
{
    if (m_repeatKeymap.find(key) == m_repeatKeymap.end())
        m_repeatKeymap[key] = false;
    return m_repeatKeymap[key];
}

bool Keyboard::IsAnyDown()
{
    for (auto&[key, state] : m_keymap)
    {
        if (state)
            return true;
    }
    return false;
}

void Keyboard::OnPress(Key key) noexcept
{
    m_keymap[key] = true;
}

void Keyboard::OnRelease(Key key) noexcept
{
    m_keymap[key] = false;
}

void Keyboard::OnRepeat(Key key) noexcept
{
    m_repeatKeymap[key] = true;
}