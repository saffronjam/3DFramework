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
    Shared<StartupLayer> _startupLayer;
    Shared<EditorLayer> _editorLayer;
};
}