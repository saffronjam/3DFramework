#pragma once

#include <memory>

#include "Graphics.h"
#include "GuiMgr.h"
#include "IEventHandler.h"

class IApp : public IEventHandler
{
public:
    IApp();
    virtual ~IApp() = default;

    void Run();
    void Exit();

protected:
    virtual void OnInit() = 0;
    virtual void OnExit() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnDraw() = 0;

    void HandleEvent(const Event &event) override;

protected:
    Window m_wnd;
    Graphics m_gfx;

public:
    class Exception : public IException
    {
    public:
        Exception(int line, const char *file, int errorCode, const char *errorString) noexcept;
        const char *what() const noexcept override;
        const char *GetType() const noexcept override;
        const char *GetErrorString() const noexcept;

    private:
        int errorCode;
        std::string errorString;
    };
};