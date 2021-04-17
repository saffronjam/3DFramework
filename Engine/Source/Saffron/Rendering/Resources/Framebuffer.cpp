#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/Framebuffer.h"
#include "Saffron/Platform/OpenGL/OpenGLFramebuffer.h"

namespace Se
{
///////////////////////////////////////////////////////////////////////////
/// Framebuffer
///////////////////////////////////////////////////////////////////////////

FramebufferTextureSpecification::FramebufferTextureSpecification(FramebufferTextureFormat format):
	TextureFormat(format)
{
}

FramebufferAttachmentSpecification::FramebufferAttachmentSpecification(
	const InitializerList<FramebufferTextureSpecification>& attachments):
	Attachments(attachments)
{
}

Shared<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
	Shared<Framebuffer> result = nullptr;

	switch (RendererApi::Current())
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

FramebufferPool::FramebufferPool(Uint32 maxFBs /* = 32 */) :
	SingleTon(this)
{
}

FramebufferPool::~FramebufferPool()
{
}

Weak<Framebuffer> FramebufferPool::AllocateBuffer()
{
	// m_Pool.push_back();
	return Weak<Framebuffer>();
}

void FramebufferPool::Add(const Shared<Framebuffer>& framebuffer)
{
	m_Pool.push_back(framebuffer);
}
}
