#include "Window.h"

#include <GLFW/glfw3.h>

Window::Window(std::string title, int width, int height)
        : m_title(std::move(title)),
          m_width(width),
          m_height(height)
{

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    m_glfwWindow = glfwCreateWindow(width, height, m_title.c_str(), nullptr, nullptr);
}
Window::~Window()
{
    glfwTerminate();
}
