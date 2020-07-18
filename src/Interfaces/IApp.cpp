
#include "IApp.h"

#include "GenericThrowMacros.h"

IApp::IApp()
        : m_wnd("Saffron Engine", 1024, 720)
{
}

void IApp::Run()
{
    try
    {
        OnInit();
    }
    LogOnly;

    while (!m_wnd.ShouldClose())
    {
        m_wnd.kbd.Update();
        m_wnd.mouse.Update();
        m_wnd.PollAllEvents();
        m_wnd.BeginFrame();
        try
        {
            OnUpdate();
            OnDraw();
        }
        LogOnly;
        m_wnd.EndFrame();
    }
}

void IApp::Exit()
{
    OnExit();
}

void IApp::HandleEvent(const Event &event)
{
}

IApp::Exception::Exception(int line, const char *file, int errorCode, const char *errorString) noexcept
        : IException(line, file),
          errorCode(errorCode),
          errorString(errorString)
{
}

const char *IApp::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Code] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *IApp::Exception::GetType() const noexcept
{
    return "Saffron Engine App Exception";
}

const char *IApp::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}