#pragma once

#include "Window.h"

class Graphics
{
public:
    explicit Graphics(Window &window);

    void BeginFrame(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void EndFrame();

    void DrawArrays();
    void DrawIndexed(const std::vector<unsigned int>& indicies);

private:
    Window &m_wnd;

};


