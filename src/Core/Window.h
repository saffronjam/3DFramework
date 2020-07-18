#pragma once

#include <string>
#include <deque>

#include "IException.h"
#include "Engine.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Event.h"

class Window
{
public:
    Window(std::string title, int width, int height, bool activeContext = true);
    ~Window();

    void BeginFrame(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void EndFrame();

    bool ShouldClose();
    void Close();

    void PollAllEvents();
    void PushEvent(const Event &event);
    void AddEventHandler(Event::Type type, IEventHandler *handler);

    [[nodiscard]] class GLFWwindow *GetCoreWindow() const noexcept;

    void SetTitle(std::string title);

private:
    void SetupGLFWCallbacks();

private:
    std::string m_title;
    int m_width, m_height;

    class GLFWwindow *m_glfwWindow;

    std::deque<Event> m_events;
    std::multimap<Event::Type, IEventHandler *> m_handlers;

public:
    Keyboard kbd;
    Mouse mouse;

public:
    class Exception : public IException
    {
    public:
        Exception(int line, const char *file, int errorCode, const char *errorString) noexcept;
        const char *what() const noexcept override;
        const char *GetType() const noexcept override;
        int GetErrorCode() const noexcept;
        const char *GetErrorString() const noexcept;

    private:
        int errorCode;
        std::string errorString;
    };
};

