#pragma once

#include "Saffron/Rendering/Resources/Framebuffer.h"

namespace Se
{
class OpenGLFramebuffer : public Framebuffer
{
public:
	explicit OpenGLFramebuffer(const FramebufferSpecification& specification);
	virtual ~OpenGLFramebuffer();

	void Resize(Uint32 width, Uint32 height, bool forceRecreate = false) override;

	void Bind() const override;
	void Unbind() const override;

	void BindTexture(Uint32 attachmentIndex = 0, Uint32 slot = 0) const override;

	Uint32 GetWidth() const override;
	Uint32 GetHeight() const override;

	RendererID GetRendererID() const override;
	RendererID GetColorAttachmentRendererID(int index = 0) const override;
	RendererID GetDepthAttachmentRendererID() const override;

	const FramebufferSpecification& GetSpecification() const override;
private:
	FramebufferSpecification m_Specification;
	RendererID m_RendererID = 0;

	ArrayList<RendererID> m_ColorAttachments;
	RendererID m_DepthAttachment;

	ArrayList<FramebufferTextureFormat> m_ColorAttachmentFormats;
	FramebufferTextureFormat m_DepthAttachmentFormat = FramebufferTextureFormat::None;

	Uint32 m_Width = 0, m_Height = 0;
};
}
