#pragma once

#include <string>

#include "IException.h"
#include "Engine.h"

class Window
{
public:
    Window(std::string title, int width, int height, bool activeContext = true);
    ~Window();

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

