#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/VertexArray.h"
#include "Saffron/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Se
{
Ref<VertexArray> VertexArray::Create()
{
	switch ( Renderer::GetAPI() )
	{
	case RendererAPI::API::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexArray>();
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
	}
}
}
