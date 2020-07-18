#pragma once

#include <memory>

#include "IApp.h"
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
};