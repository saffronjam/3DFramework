#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Image.h"

namespace Se
{
struct FramebufferAttachmentSpec
{
	FramebufferAttachmentSpec(std::initializer_list<ImageFormat> formats) :
		Formats(formats)
	{
	}

	std::vector<ImageFormat> Formats;
};

struct FramebufferSpec
{
	uint Width, Height;
	FramebufferAttachmentSpec AttachmentSpec;
	Color ClearColor = {0.0f, 0.0f, 0.0f, 0.0f};
};

class Framebuffer : public Bindable
{
public:
	explicit Framebuffer(const FramebufferSpec& spec);

	void Bind() override;

	virtual void Resize(uint width, uint height);
	void Clear();

	auto TargetByIndex(uint index) const -> const Image&;
	auto FinalTarget() const -> const Image&;
	auto DepthTarget() const -> const Image&;

	static auto Create(const FramebufferSpec& spec) -> std::shared_ptr<Framebuffer>;

protected:
	Framebuffer() = default;

protected:
	uint _width = 0, _height = 0;
	Color _clearColor;
	
	std::vector<ImageFormat> _colorAttachmentFormats;
	ImageFormat _depthStencilAttachmentFormat = ImageFormat::None;

	std::vector<std::shared_ptr<Image>> _colorAttachments;
	std::shared_ptr<Image> _depthStencilAttachment;

	std::vector<ID3D11RenderTargetView*> _nativeRenderTargetViews;
};

namespace Utils
{
inline auto IsDepthFormat(ImageFormat format)
{
	return format == ImageFormat::Depth24Stencil8 || format == ImageFormat::Depth32f;
}
}
}
