#include "App.h"

void App::OnInit()
{
    m_drawables.push_back(std::make_unique<TestTriangle>());
}

void App::OnExit()
{
}

void App::OnUpdate()
{
    for(auto& drawable : m_drawables)
    {
        drawable->Update(m_wnd.mouse);
    }
}

void App::OnDraw()
{
    for(auto& drawable : m_drawables)
    {
        drawable->Draw(m_gfx);
    }
}
