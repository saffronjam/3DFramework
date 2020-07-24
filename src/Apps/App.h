#pragma once

#include <memory>

#include "IApp.h"
#include "Camera.h"
#include "TestBox.h"
#include "TestTriangle.h"

class App : public IApp
{
public:
    void OnInit() override;
    void OnExit() override;
    void OnUpdate() override;
    void OnDraw() override;

private:
    std::vector<std::unique_ptr<Drawable>> m_drawables;

    float m_windowTitleUpdateTimer = 0.0f;

    int nDrawables = 25;
    glm::vec3 trans = {0.0f, 0.0f, 0.0f};
    glm::vec3 rot = {0.0f, 0.0f, 0.0f};
    glm::vec3 color = {0.0f, 0.0f, 0.0f};

    Camera m_camera;
};