#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/Framebuffer.h"
#include "Saffron/Platform/OpenGL/OpenGLFramebuffer.h"

namespace Se
{
///////////////////////////////////////////////////////////////////////////
/// Framebuffer
///////////////////////////////////////////////////////////////////////////

Shared<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
	Shared<Framebuffer> result = nullptr;

	switch (RendererAPI::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: result = Shared<OpenGLFramebuffer>::Create(spec);
	}
	FramebufferPool::GetGlobal()->Add(result);
	return result;
}

///////////////////////////////////////////////////////////////////////////
/// Framebuffer Pool
///////////////////////////////////////////////////////////////////////////

FramebufferPool* FramebufferPool::s_Instance = new FramebufferPool;

FramebufferPool::FramebufferPool(uint32_t maxFBs /* = 32 */) :
	SingleTon(this)
{
}

FramebufferPool::~FramebufferPool()
{
}

std::weak_ptr<Framebuffer> FramebufferPool::AllocateBuffer()
{
	// m_Pool.push_back();
	return std::weak_ptr<Framebuffer>();
}

void FramebufferPool::Add(const Shared<Framebuffer>& framebuffer)
{
	m_Pool.push_back(framebuffer);
}
}
