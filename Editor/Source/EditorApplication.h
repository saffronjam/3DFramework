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
    std::shared_ptr<StartupLayer> m_StartupLayer;
    std::shared_ptr<EditorLayer> m_EditorLayer;
};
}