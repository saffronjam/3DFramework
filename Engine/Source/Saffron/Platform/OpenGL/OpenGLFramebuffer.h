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
	FramebufferSpecification _specification;
	RendererID _rendererID = 0;

	ArrayList<RendererID> _colorAttachments;
	RendererID _depthAttachment;

	ArrayList<FramebufferTextureFormat> _colorAttachmentFormats;
	FramebufferTextureFormat _depthAttachmentFormat = FramebufferTextureFormat::None;

	Uint32 _width = 0, _height = 0;
};
}
