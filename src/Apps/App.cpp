#include "App.h"

#include "imgui/imgui.h"

void App::OnInit()
{
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);

    const int nDrawables = 25;
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
    for (auto &drawable : m_drawables)
    {
        drawable->Update(m_wnd.mouse);
    }
    ImGui::Begin("Box controls");
    ImGui::SliderFloat3("Rotation", &rot.x, 0, 2 * PI<>, "%.2f");
    ImGui::SliderFloat3("Translation", &trans.x, -5.0f, 5.0f, "%.2f");
    ImGui::End();
    m_drawables[0]->SetRotation(rot.x, rot.y, rot.z);
    m_drawables[0]->SetTranslation(trans);
    const float rotIntensity = 0.01f;
    for (auto &drawable : m_drawables)
    {
        float yaw = Random::Real(0.0f, rotIntensity);
        float pitch = Random::Real(0.0f, rotIntensity);
        float roll = Random::Real(0.0f, rotIntensity);
        drawable->Rotate(yaw, pitch, roll);
    }

}

void App::OnDraw()
{
    for (auto &drawable : m_drawables)
    {
        drawable->Draw(m_gfx);
    }
}
