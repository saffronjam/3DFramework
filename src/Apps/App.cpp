#include "App.h"

#include "imgui/imgui.h"

void App::OnInit()
{
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);

    const int nDrawables = 500;
    for (int i = 0; i < nDrawables; i++)
    {
        auto randomVec = Random::Vec3(glm::vec3{-10.0f, -10.0f, -45.0f}, glm::vec3{10.0f, 10.0f, -15.0f});
        glm::mat4 transform = glm::translate(randomVec);
        m_drawables.push_back(std::make_unique<TestBox>(transform));
    }
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
    m_windowTitleUpdateTimer += dt;
    if (m_windowTitleUpdateTimer > 0.5f)
    {
        std::ostringstream windowTitle;
        windowTitle << "FPS: " << 1.0f / dt;
        m_wnd.SetTitle(windowTitle.str());
        m_windowTitleUpdateTimer = 0.0f;
    }

    const float rotIntensity = 0.01f;
    for (auto &drawable : m_drawables)
    {
        float yaw = Random::Real(0.0f, rotIntensity);
        float pitch = Random::Real(0.0f, rotIntensity);
        float roll = Random::Real(0.0f, rotIntensity);
        drawable->Rotate(yaw, pitch, roll);
    }
    ImGui::Begin("Box controls");
    ImGui::SliderFloat3("Rotation", &rot.x, 0, 2 * PI<>, "%.2f");
    ImGui::SliderFloat3("Translation", &trans.x, -5.0f, 5.0f, "%.2f");
    ImGui::SliderFloat3("Color", &color.x, 0.0f, 1.0f, "%.2f");
    ImGui::SliderInt("Num Drawables", &nDrawables, 0, 500);
    ImGui::End();
    m_drawables[0]->SetRotation(rot.x, rot.y, rot.z);
    m_drawables[0]->SetTranslation(trans);

    for (size_t i = 0; i < nDrawables; i++)
    {
        m_drawables[i]->SetExtraColor(color);
        m_drawables[i]->Update(m_wnd.mouse);
    }
}

void App::OnDraw()
{
    for (size_t i = 0; i < nDrawables; i++)
    {
        m_drawables[i]->Draw(m_gfx);
    }
}
