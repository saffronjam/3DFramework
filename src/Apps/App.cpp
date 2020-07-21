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

    float speed = 0.001f;
    float yaw = (m_wnd.kbd.IsDown(Keyboard::Q) - m_wnd.kbd.IsDown(Keyboard::W)) * speed;
    float pitch = (m_wnd.kbd.IsDown(Keyboard::A) - m_wnd.kbd.IsDown(Keyboard::S)) * speed;
    float roll = (m_wnd.kbd.IsDown(Keyboard::Z) - m_wnd.kbd.IsDown(Keyboard::X)) * speed;
    m_drawables[0]->Rotate(yaw, pitch, roll);
}

void App::OnDraw()
{
    for (auto &drawable : m_drawables)
    {
        drawable->Draw(m_gfx);
    }
}
