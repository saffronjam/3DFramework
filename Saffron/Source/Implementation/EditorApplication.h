#pragma once

#include <Saffron.h>

#include "Implementation/EditorLayer.h"
#include "Implementation/StartupLayer.h"

namespace Se
{
class EditorApplication : public Application {
public:
    explicit EditorApplication(const Properties &props);

    void OnInit() override;

    void OnUpdate() override;

private:
    std::shared_ptr<StartupLayer> m_StartupLayer;
    std::shared_ptr<EditorLayer> m_EditorLayer;
};
}