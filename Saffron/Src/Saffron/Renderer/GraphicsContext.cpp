#include "Saffron/SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Renderer/GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Se
{
Scope<GraphicsContext> GraphicsContext::Create(void *window)
{
	//switch ( Renderer::GetAPI() )
	//{
	//case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	//case RendererAPI::API::OpenGL:  
	//}

	return CreateScope<OpenGLContext>(static_cast<GLFWwindow *>(window));

	SE_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}
}
