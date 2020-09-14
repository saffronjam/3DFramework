#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/Framebuffer.h"
#include "Saffron/Platform/OpenGL/OpenGLFramebuffer.h"

namespace Se
{

///////////////////////////////////////////////////////////////////////////
/// Framebuffer
///////////////////////////////////////////////////////////////////////////

Ref<Framebuffer> Framebuffer::Create(const Specification &spec)
{
	Ref<Framebuffer> result = nullptr;

	switch ( RendererAPI::CurrentAPI() )
	{
	case RendererAPI::Type::None:		return nullptr;
	case RendererAPI::Type::OpenGL:		result = Ref<OpenGLFramebuffer>::Create(spec);
	default:							SE_CORE_ASSERT(false, "Unknown RendererAPI"); result = nullptr;
	}

	if ( !result )
		FramebufferPool::GetGlobal()->Add(result);

	return result;
}


///////////////////////////////////////////////////////////////////////////
/// Framebuffer Pool
///////////////////////////////////////////////////////////////////////////

FramebufferPool *FramebufferPool::sInstance = new FramebufferPool;

std::weak_ptr<Framebuffer> FramebufferPool::AllocateBuffer()
{
	// m_Pool.push_back();
	return std::weak_ptr<Framebuffer>();
}

void FramebufferPool::Add(const Ref<Framebuffer> &framebuffer)
{
}
}
