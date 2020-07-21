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

    float rotSpeed = 0.001f;
    float yaw = (m_wnd.kbd.IsDown(Keyboard::Q) - m_wnd.kbd.IsDown(Keyboard::W)) * rotSpeed;
    float pitch = (m_wnd.kbd.IsDown(Keyboard::A) - m_wnd.kbd.IsDown(Keyboard::S)) * rotSpeed;
    float roll = (m_wnd.kbd.IsDown(Keyboard::Z) - m_wnd.kbd.IsDown(Keyboard::X)) * rotSpeed;
    m_drawables[0]->Rotate(yaw, pitch, roll);

    float transSpeed = 0.001f;
    float transX = (m_wnd.kbd.IsDown(Keyboard::T) - m_wnd.kbd.IsDown(Keyboard::Y)) * transSpeed;
    float transY = (m_wnd.kbd.IsDown(Keyboard::G) - m_wnd.kbd.IsDown(Keyboard::H)) * transSpeed;
    float transZ = (m_wnd.kbd.IsDown(Keyboard::B) - m_wnd.kbd.IsDown(Keyboard::N)) * transSpeed;
    m_drawables[0]->Translate(glm::vec3(transX, transY, transZ));

    ImGui::ShowDemoWindow();

}

void App::OnDraw()
{
    for (auto &drawable : m_drawables)
    {
        drawable->Draw(m_gfx);
    }
}
