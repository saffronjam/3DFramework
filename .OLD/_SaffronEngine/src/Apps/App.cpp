#include "App.h"

#include "imgui/imgui.h"

void App::OnInit()
{
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);

    m_drawables.push_back(std::make_unique<TestSolidBox>(glm::mat4(1)));

    const int nDrawables = 501;
    for (int i = 1; i < nDrawables; i++)
    {
        auto randomVec = Random::Vec3(glm::vec3{-10.0f, -10.0f, -10.0f}, glm::vec3{10.0f, 10.0f, 10.0f});
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
    m_camera.Update(dt, m_wnd.mouse, m_wnd.kbd);
    UpdateWindowTitle();

    ImGui::Begin("Box controls");
    ImGui::SliderFloat3("Values", &values.x, 0.0f, 1.0f, "%.2f");
    ImGui::SliderInt("Num Drawables", &nDrawables, 1, 500);
    ImGui::End();

    ImGui::Begin("Light controls");
    ImGui::SliderFloat3("Position", &lightPos.x, -60.0f, 60.0f, "%.1f");

    ImGui::Text("Intensity/Color");
    ImGui::ColorEdit3("Light Color", &lightColor.x);
    ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.01f, 2.0f, "%.2f", 2);
    ImGui::SliderFloat("Diffuse Intensity", &diffuseIntensity, 0.01f, 2.0f, "%.2f", 2);
    ImGui::SliderFloat("Specular Intensity", &specularIntensity, 0.01f, 20.0f, "%.2f", 2);;
    ImGui::SliderFloat("Specular Power", &specularPower, 16.0f, 512.0f, "%.0f", 2);;

    ImGui::Text("Falloff");
    ImGui::SliderFloat("Constant", &attConst, 0.05f, 10.0f, "%.2f", 4);
    ImGui::SliderFloat("Linear", &attLin, 0.0001f, 4.0f, "%.4f", 8);
    ImGui::SliderFloat("Quadratic", &attQuad, 0.0000001f, 10.0f, "%.7f", 10);
    ImGui::End();

    m_drawables[0]->SetTranslation(lightPos);
    m_drawables[0]->SetView(m_camera.GetMatrix());
    m_drawables[0]->SetExtraValues(values);
    m_drawables[0]->Update(m_wnd.mouse, m_camera);

    for (size_t i = 1; i < nDrawables; i++)
    {
        m_drawables[i]->lightPos = lightPos;
        m_drawables[i]->lightColor = lightColor;
        m_drawables[i]->ambientIntensity = ambientIntensity;
        m_drawables[i]->diffuseIntensity = diffuseIntensity;
        m_drawables[i]->specularIntensity = specularIntensity;
        m_drawables[i]->specularPower = specularPower;
        m_drawables[i]->attConst = attConst;
        m_drawables[i]->attLin = attLin;
        m_drawables[i]->attQuad = attQuad;
//        m_drawables[i]->Rotate(dt, dt * 1.5f, dt * 0.5f);
        m_drawables[i]->SetView(m_camera.GetMatrix());
        m_drawables[i]->SetExtraValues(values);
        m_drawables[i]->Update(m_wnd.mouse, m_camera);
    }
}

void App::OnDraw()
{
    for (size_t i = 0; i < nDrawables; i++)
    {
        m_drawables[i]->Draw(m_gfx);
    }
}

void App::UpdateWindowTitle()
{
    m_windowTitleUpdateTimer += dt;
    if (m_windowTitleUpdateTimer > 0.5f)
    {
        std::ostringstream windowTitle;
#ifdef SAFFRON_DEBUG
        const char *buildType = "Debug";
#else
        const char *buildType = "Release";
#endif
        windowTitle << "[ " << buildType << " ]  ";
        windowTitle << "FPS: " << 1.0f / dt;
        m_wnd.SetTitle(windowTitle.str());
        m_windowTitleUpdateTimer = 0.0f;
    }
}
