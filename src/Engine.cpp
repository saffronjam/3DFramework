#include "Engine.h"

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void Engine::Init(Engine::Subsystem subsystem)
{
    glfwSetErrorCallback(error_callback);
    glewInit();
    glfwInit();
}
