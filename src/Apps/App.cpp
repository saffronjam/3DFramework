#include "App.h"

#include "imgui/imgui.h"

void App::OnInit()
{
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);
    m_drawables.push_back(std::make_unique<TestBox>());
    for (auto &drawable : m_drawables)
    {
        drawable->SetProjection(proj);
    }
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
    ImGui::Begin("Box controls");
    ImGui::SliderFloat3("Rotation", &rot.x, 0, 2 * PI<>, "%.2f");
    ImGui::SliderFloat3("Translation", &trans.x, -20.0f, 20.0f, "%.2f");
    ImGui::End();
    m_drawables[0]->SetRotation(rot.x, rot.y, rot.z);
    m_drawables[0]->SetTranslation(trans);

}

void App::OnDraw()
{
    for (auto &drawable : m_drawables)
    {
        drawable->Draw(m_gfx);
    }
}
