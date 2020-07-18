#pragma once

#include <memory>

#include "IApp.h"

class App : public IApp
{
public:
    void OnInit() override;
    void OnExit() override;
    void OnUpdate() override;
    void OnDraw() override;
};