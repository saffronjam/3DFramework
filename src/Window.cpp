#include "Window.h"

#include <GLFW/glfw3.h>

Window::Window(std::string title, int width, int height, bool activeContext)
        : m_title(std::move(title)),
          m_width(width),
          m_height(height)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

    Engine::BindErrorCallback([](int code, const char *desc)
                              {
                                  throw Window::Exception(__LINE__, __FILE__, code, desc);
                              });
    m_glfwWindow = glfwCreateWindow(width, height, m_title.c_str(), nullptr, nullptr);

    if (activeContext)
    {
        glfwMakeContextCurrent(m_glfwWindow);
    }
}

Window::~Window()
{
    glfwDestroyWindow(m_glfwWindow);
}

Window::Exception::Exception(int line, const char *file, int errorCode, const char *errorString) noexcept
        : IException(line, file),
          errorCode(errorCode),
          errorString(errorString)
{
}

const char *Window::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Code] " << GetErrorCode() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *Window::Exception::GetType() const noexcept
{
    return "Saffron Window Exception";
}

int Window::Exception::GetErrorCode() const noexcept
{
    return errorCode;
}

const char *Window::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}
