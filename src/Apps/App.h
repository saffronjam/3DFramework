#pragma once

#include <memory>

#include "IApp.h"
#include "Camera.h"
#include "TestBox.h"
#include "TestSolidBox.h"
#include "TestTriangle.h"

class App : public IApp
{
public:
    void OnInit() override;
    void OnExit() override;
    void OnUpdate() override;
    void OnDraw() override;

private:
    void UpdateWindowTitle();

private:
    std::vector<std::unique_ptr<Drawable>> m_drawables;

    float m_windowTitleUpdateTimer = 0.0f;

    int nDrawables = 25;
    glm::vec3 values = {0.0f, 0.0f, 0.0f};

    //tmp
    glm::vec3 lightPos = {0.0f, 0.0f, 0.0f};
    glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};
    float ambientIntensity = 0.3f;
    float diffuseIntensity = 0.3f;
    float specularIntensity = 0.3f;
    float specularPower = 512.0f;
    float attConst = 0.3f;
    float attLin = 0.3f;
    float attQuad = 0.3f;

    Camera m_camera;
};