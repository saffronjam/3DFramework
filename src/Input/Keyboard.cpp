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
    case Action::Press:
        OnPress(event.keyboard);
        break;
    case Action::Release:
        OnRelease(event.keyboard);
        break;
    case Action::Repeat:
        OnRepeat(event.keyboard);
        break;
    default:
        break;
    }
}

bool Keyboard::IsDown(Key key) const noexcept
{
    if (m_keymap.find(key) == m_keymap.end())
        m_keymap[key] = false;
    return m_keymap[key];
}

bool Keyboard::WasDown(Keyboard::Key key) const noexcept
{
    if (m_prevKeymap.find(key) == m_prevKeymap.end())
        m_prevKeymap[key] = false;
    return m_prevKeymap[key];
}

bool Keyboard::IsPressed(Key key) const noexcept
{
    return IsDown(key) && !WasDown(key);
}

bool Keyboard::IsReleased(Key key) const noexcept
{
    return !IsDown(key) && WasDown(key);
}

bool Keyboard::IsBeingRepeated(Key key) const noexcept
{
    if (m_repeatKeymap.find(key) == m_repeatKeymap.end())
        m_repeatKeymap[key] = false;
    return m_repeatKeymap[key];
}

bool Keyboard::IsAnyDown() const noexcept
{
    for (auto&[key, state] : m_keymap)
    {
        if (state)
            return true;
    }
    return false;
}

void Keyboard::OnPress(const KeyboardEvent &event)
{
    m_keymap[event.key] = true;
    for (auto &callback : m_callbacks[Action::Press])
        callback(event);
}

void Keyboard::OnRelease(const KeyboardEvent &event)
{
    m_keymap[event.key] = false;
    for (auto &callback : m_callbacks[Action::Release])
        callback(event);
}

void Keyboard::OnRepeat(const KeyboardEvent &event)
{
    m_repeatKeymap[event.key] = true;
    for (auto &callback : m_callbacks[Action::Repeat])
        callback(event);
}