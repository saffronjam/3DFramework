#include "Engine.h"
#include "Window.h"

#include <GLFW/glfw3.h>

int main()
{
    Engine::Initialize(Engine::Graphics);
    Window window("Saffron Engine", 1024, 720);

    while (!glfwWindowShouldClose(window.GetCoreWindow()))
    {
        window.BeginFrame();
        window.kbd.Update();
        window.mouse.Update();
        if (window.kbd.IsReleased(Keyboard::E))
            window.SetTitle("button down");
        else
            window.SetTitle("button up");
        window.EndFrame();
    }
}