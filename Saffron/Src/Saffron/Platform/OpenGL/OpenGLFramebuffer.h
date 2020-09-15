#pragma once

#include "Saffron/Renderer/Framebuffer.h"

namespace Se
{
class OpenGLFramebuffer : public Framebuffer
{
public:
	OpenGLFramebuffer(const Specification &spec);
	virtual ~OpenGLFramebuffer();

	void Resize(Uint32 width, Uint32 height, bool forceRecreate = false) override;

	void Bind() const override;
	void Unbind() const override;

	void BindTexture(Uint32 slot = 0) const override;

	virtual RendererID GetRendererID() const { return m_RendererID; }
	virtual RendererID GetColorAttachmentRendererID() const { return m_ColorAttachment; }
	virtual RendererID GetDepthAttachmentRendererID() const { return m_DepthAttachment; }

	const Specification &GetSpecification() const override { return m_Specification; }
private:
	Specification m_Specification;
	RendererID m_RendererID = 0;
	RendererID m_ColorAttachment = 0, m_DepthAttachment = 0;
};
}

