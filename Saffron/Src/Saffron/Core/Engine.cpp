#include "Saffron/SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Core/Engine.h"
#include "Saffron/System/Log.h"

namespace Se
{
void Engine::Initialize(Engine::Subsystem subsystem)
{
	BindDefaultErrorCallback();
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

void Engine::BindDefaultErrorCallback()
{
	glfwSetErrorCallback([](int error, const char *desc)
						 {
							 SE_WARN("Unexcepted error [{0:d}] Description: {1}", error, desc);
						 });
}

void Engine::UnbindErrorCallback()
{
	glfwSetErrorCallback(nullptr);
}
}
