#include "Engine.h"

#include <cstdio>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void Engine::Initialize(Engine::Subsystem subsystem)
{
    glewInit();
    glfwInit();
}

void Engine::Uninitialize(Engine::Subsystem subsystem)
{
    glfwTerminate();
}
void Engine::BindErrorCallback(GLFWerrorfun callback)
{
    glfwSetErrorCallback(callback);
}

void Engine::UnbindErrorCallback()
{
    glfwSetErrorCallback(nullptr);
}
