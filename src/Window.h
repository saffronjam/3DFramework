#pragma once

#include <string>

class Window
{
public:
    Window(std::string title, int width, int height);
    ~Window();


private:
    class GLFWwindow* m_glfwWindow;
    std::string m_title;
    int m_width, m_height;
};

