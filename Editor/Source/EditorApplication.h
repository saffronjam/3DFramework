#pragma once

#include <Saffron.h>

#include "Layers/EditorLayer.h"
#include "Layers/StartupLayer.h"

namespace Se
{
class EditorApplication : public App {
public:
    explicit EditorApplication(const Properties &props);

    void OnInit() override;

    void OnUpdate() override;

private:
    Shared<StartupLayer> m_StartupLayer;
    Shared<EditorLayer> m_EditorLayer;
};
}