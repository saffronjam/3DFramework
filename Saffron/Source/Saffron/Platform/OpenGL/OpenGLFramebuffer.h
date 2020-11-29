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

	void BindTexture(Uint32 attachmentIndex = 0, Uint32 slot = 0) const override;

	Uint32 GetWidth() const override { return m_Width; }
	Uint32 GetHeight() const override { return m_Height; }

	RendererID GetRendererID() const override { return m_RendererID; }
	RendererID GetColorAttachmentRendererID(int index = 0) const override { return m_ColorAttachments[index]; }
	RendererID GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

	const Specification &GetSpecification() const override { return m_Specification; }
private:
	Specification m_Specification;
	RendererID m_RendererID = 0;

	ArrayList<RendererID> m_ColorAttachments;
	RendererID m_DepthAttachment;

	ArrayList<TextureFormat> m_ColorAttachmentFormats;
	TextureFormat m_DepthAttachmentFormat = TextureFormat::None;

	Uint32 m_Width = 0, m_Height = 0;
};
}

