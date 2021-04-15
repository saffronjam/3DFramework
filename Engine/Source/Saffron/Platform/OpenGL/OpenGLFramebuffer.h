#pragma once

#include "Saffron/Rendering/Resources/Framebuffer.h"

namespace Se
{
class OpenGLFramebuffer : public Framebuffer
{
public:
	explicit OpenGLFramebuffer(const FramebufferSpecification& specification);
	virtual ~OpenGLFramebuffer();

	void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;

	void Bind() const override;
	void Unbind() const override;

	void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const override;

	uint32_t GetWidth() const override;
	uint32_t GetHeight() const override;

	RendererID GetRendererID() const override;
	RendererID GetColorAttachmentRendererID(int index = 0) const override;
	RendererID GetDepthAttachmentRendererID() const override;

	const FramebufferSpecification& GetSpecification() const override;
private:
	FramebufferSpecification m_Specification;
	RendererID m_RendererID = 0;

	std::vector<RendererID> m_ColorAttachments;
	RendererID m_DepthAttachment;

	std::vector<FramebufferTextureFormat> m_ColorAttachmentFormats;
	FramebufferTextureFormat m_DepthAttachmentFormat = FramebufferTextureFormat::None;

	uint32_t m_Width = 0, m_Height = 0;
};
}
