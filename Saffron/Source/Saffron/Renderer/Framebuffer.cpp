#include "SaffronPCH.h"

#include "Saffron/Renderer/Framebuffer.h"
#include "Saffron/Platform/OpenGL/OpenGLFramebuffer.h"

namespace Se
{

///////////////////////////////////////////////////////////////////////////
/// Framebuffer
///////////////////////////////////////////////////////////////////////////

Shared<Framebuffer> Framebuffer::Create(const Specification &spec)
{
	Shared<Framebuffer> result = nullptr;

	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:		SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
	case RendererAPI::Type::OpenGL:		result = Shared<OpenGLFramebuffer>::Create(spec); break;
	default:							SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;;
	}

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

void FramebufferPool::Add(const Shared<Framebuffer> &framebuffer)
{
}
}
