#include "Saffron/SaffronPCH.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Saffron/System/Log.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Se
{

OpenGLContext::OpenGLContext(GLFWwindow *windowHandle)
	: m_WindowHandle(windowHandle)
{
	SE_CORE_ASSERT(windowHandle, "Window handle is null");
	glfwMakeContextCurrent(m_WindowHandle);
	const auto success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	SE_CORE_ASSERT(success, "Failed to initialize Glad");

	SE_CORE_INFO("--- OpenGL Info ---");
	SE_CORE_INFO(" Vendor: {0}", glGetString(GL_VENDOR));
	SE_CORE_INFO(" Renderer: {0}", glGetString(GL_RENDERER));
	SE_CORE_INFO(" Version: {0}", glGetString(GL_VERSION));
	SE_CORE_INFO("-------------------");
}

void OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_WindowHandle);
}

}
