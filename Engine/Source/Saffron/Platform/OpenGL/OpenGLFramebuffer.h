#pragma once

#include "Saffron/Rendering/Resources/Framebuffer.h"

namespace Se
{
class OpenGLFramebuffer : public Framebuffer
{
public:
	explicit OpenGLFramebuffer(const FramebufferSpecification& specification);
	virtual ~OpenGLFramebuffer();

	void Resize(uint width, uint height, bool forceRecreate = false) override;

	void Bind() const override;
	void Unbind() const override;

	void Clear() override;

	void BindTexture(uint attachmentIndex = 0, uint slot = 0) const override;

	uint GetWidth() const override;
	uint GetHeight() const override;

	RendererID GetRendererID() const override;
	RendererID GetColorAttachmentRendererID(int index = 0) const override;
	RendererID GetDepthAttachmentRendererID() const override;

	const FramebufferSpecification& GetSpecification() const override;
private:
	FramebufferSpecification _specification;
	RendererID _rendererID = 0;

	List<RendererID> _colorAttachments;
	RendererID _depthAttachment;

	List<FramebufferTextureFormat> _colorAttachmentFormats;
	FramebufferTextureFormat _depthAttachmentFormat = FramebufferTextureFormat::None;

	uint _width = 0, _height = 0;
};
}
