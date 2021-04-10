#pragma once

#include <Saffron.h>

#include "Layers/ProjectLayer.h"

namespace Se
{
class EditorApplication : public App {
public:
    explicit EditorApplication(const Properties &props);

    void OnInit() override;

    void OnUpdate() override;

    void OnShutdown() override;

private:
    Shared<ProjectLayer> _projectLayer;
};
}