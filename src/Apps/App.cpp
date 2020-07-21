#include "App.h"

void App::OnInit()
{
    m_drawables.push_back(std::make_unique<TestBox>());
}

void App::OnExit()
{
}

void App::OnUpdate()
{
    for (auto &drawable : m_drawables)
    {
        drawable->Update(m_wnd.mouse);
    }

    if (m_wnd.kbd.IsDown(Keyboard::Q))
    {
        m_drawables[0]->Rotate(0.01f, 0.0f, 0.0f);
    }
    if (m_wnd.kbd.IsDown(Keyboard::W))
    {
        m_drawables[0]->Rotate(-0.01f, 0.0f, 0.0f);
    }
    if (m_wnd.kbd.IsDown(Keyboard::A))
    {
        m_drawables[0]->Rotate(0.0f, 0.01f, 0.0f);
    }
    if (m_wnd.kbd.IsDown(Keyboard::S))
    {
        m_drawables[0]->Rotate(0.0f, -0.01f, 0.0f);
    }
    if (m_wnd.kbd.IsDown(Keyboard::Z))
    {
        m_drawables[0]->Rotate(0.0f, 0.0f, 0.01f);
    }
    if (m_wnd.kbd.IsDown(Keyboard::X))
    {
        m_drawables[0]->Rotate(0.0f, 0.0f, -0.01f);
    }
}

void App::OnDraw()
{
    for (auto &drawable : m_drawables)
    {
        drawable->Draw(m_gfx);
    }
}
