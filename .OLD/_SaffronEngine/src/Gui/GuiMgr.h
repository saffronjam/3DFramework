#pragma once

class GuiMgr
{
public:
    GuiMgr(class Window &window);
    ~GuiMgr();

    void BeginFrame();
    void EndFrame();

private:
    class Window& m_window;
};


