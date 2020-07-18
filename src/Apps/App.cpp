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
}

void App::OnDraw()
{
    for(auto& drawable : m_drawables)
    {
        drawable->Draw(m_gfx);
    }
}
