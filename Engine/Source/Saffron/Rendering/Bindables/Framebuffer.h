#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Event/Event.h"
#include "Saffron/Rendering/Bindable.h"
#include "Saffron/Rendering/Image.h"


namespace Se
{
struct FrameBufferAttachmentElement
{
	FrameBufferAttachmentElement(ImageFormat format, int arraySize = 1) :
		Format(format),
		ArraySize(arraySize)
	{
	}

	ImageFormat Format;
	int ArraySize;
};

struct FramebufferAttachmentSpec
{
	FramebufferAttachmentSpec(std::initializer_list<FrameBufferAttachmentElement> formats)
		: Formats(formats)
	{
	}

	std::vector<FrameBufferAttachmentElement> Formats{};
};

struct FramebufferSpec
{
	uint Width, Height;
	FramebufferAttachmentSpec AttachmentSpec;
	Color ClearColor = {0.0f, 0.0f, 0.0f, 1.0f};
};

class Framebuffer : public Bindable
{
public:
	explicit Framebuffer(const FramebufferSpec& spec);

	void Bind() const override;
	void Unbind() const override;

	virtual void Resize(uint width, uint height);
	void Clear();

	auto Width() const -> uint;
	auto Height() const -> uint;

	auto ImageByIndex(uint index) -> Image&;
	auto ImageByIndex(uint index) const -> const Image&;
	auto ImagePtrByIndex(uint index) const -> const std::shared_ptr<Image>&;
	auto DepthImage() -> Image&;
	auto DepthImage() const -> const Image&;
	auto DepthImagePtr() const -> const std::shared_ptr<Image>&;

	void SetDepthClearValue(float value);

	static auto Create(const FramebufferSpec& spec) -> std::shared_ptr<Framebuffer>;

protected:
	Framebuffer() = default;

public:
	mutable SubscriberList<const SizeEvent&> Resized;

protected:
	uint _width = 0, _height = 0;
	Color _clearColor;

	std::vector<FrameBufferAttachmentElement> _colorAttachmentFormats;
	FrameBufferAttachmentElement _depthStencilAttachmentFormat = {ImageFormat::None, 1};

	std::vector<std::shared_ptr<Image>> _colorAttachments;
	std::shared_ptr<Image> _depthStencilAttachment;

	std::vector<ID3D11RenderTargetView*> _nativeRenderTargetViews;

	float _depthClearValue = 1.0f;
	uchar _stencilClearValue = 0;
};

namespace Utils
{
inline auto IsDepthFormat(ImageFormat format)
{
	return format == ImageFormat::Depth24Stencil8 || format == ImageFormat::Depth32f;
}
}
}
