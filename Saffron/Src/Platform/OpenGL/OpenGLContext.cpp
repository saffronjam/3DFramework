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
}

void OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_WindowHandle);
	const auto success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	SE_CORE_ASSERT(success, "Failed to initialize Glad");
	if ( success )
	{
		SE_CORE_INFO("Sucessfully initialized Glad");
	}
}

void OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_WindowHandle);
}

}
