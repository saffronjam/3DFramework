#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Se
{
RendererAPI::API RendererAPI::m_sAPI = API::OpenGL;

RendererAPI::API RendererAPI::GetAPI()
{
	return m_sAPI;
}

Scope<RendererAPI> RendererAPI::Create()
{
	switch ( m_sAPI )
	{
	case RendererAPI::API::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::API::OpenGL:	return CreateScope<OpenGLRendererAPI>();
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
	}
}
}
