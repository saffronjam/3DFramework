#pragma once

#include <string>

#include "IException.h"
#include "Engine.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
    Window(std::string title, int width, int height, bool activeContext = true);
    ~Window();

    void BeginFrame(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void EndFrame();

    [[nodiscard]] class GLFWwindow *GetCoreWindow() const noexcept;

    void SetTitle(std::string title);

private:
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow *window, int button, int action, int mods);

public:
    Keyboard kbd;
    Mouse mouse;

private:
    class GLFWwindow *m_glfwWindow;

    std::string m_title;
    int m_width, m_height;

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

