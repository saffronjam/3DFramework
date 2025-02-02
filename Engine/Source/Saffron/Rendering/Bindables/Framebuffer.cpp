﻿#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/Framebuffer.h"

#include <ranges>

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
Framebuffer::Framebuffer(const FramebufferSpec& spec) :
	_clearColor(spec.ClearColor),
	_colorAttachmentFormats(spec.AttachmentSpec.Formats)
{
	const uint width = spec.Width, height = spec.Height;
	SetInitializer(
		[this, width, height]
		{
			const auto inst = ShareThisAs<Framebuffer>();
			Renderer::Submit(
				[inst](const RendererPackage& package)
				{
					const auto iter = std::ranges::find_if(
						inst->_colorAttachmentFormats,
						[&inst](const FrameBufferAttachmentElement& el)
						{
							return Utils::IsDepthFormat(el.Format);
						}
					);

					if (iter != inst->_colorAttachmentFormats.end())
					{
						inst->_depthStencilAttachmentFormat = *iter;
					}

					std::erase_if(
						inst->_colorAttachmentFormats,
						[](const FrameBufferAttachmentElement& el)
						{
							return Utils::IsDepthFormat(el.Format);
						}
					);
				}
			);
			Resize(width, height);
		}
	);
}

void Framebuffer::Bind() const
{
	const auto inst = ShareThisAs<const Framebuffer>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			auto* ds = inst->_depthStencilAttachmentFormat.Format == ImageFormat::None
				           ? nullptr
				           : &inst->_depthStencilAttachment->RenderViewAs<ID3D11DepthStencilView>();
			auto* const* rs = inst->_colorAttachments.empty() ? nullptr : inst->_nativeRenderTargetViews.data();
			package.Context.OMSetRenderTargets(inst->_nativeRenderTargetViews.size(), rs, ds);
		}
	);
}

void Framebuffer::Unbind() const
{
	const auto inst = ShareThisAs<const Framebuffer>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			ID3D11RenderTargetView* nullRt = {nullptr};
			ID3D11DepthStencilView* nullDs = nullptr;
			package.Context.OMSetRenderTargets(1, &nullRt, nullDs);
		}
	);
}

void Framebuffer::Resize(uint width, uint height)
{
	const auto inst = ShareThisAs<Framebuffer>();
	Renderer::Submit(
		[=](const RendererPackage& package)
		{
			_colorAttachments.clear();
			_nativeRenderTargetViews.clear();

			if (!_colorAttachmentFormats.empty())
			{
				_colorAttachments.resize(_colorAttachmentFormats.size());
				int index = 0;
				constexpr auto usage = ImageUsage_RenderTarget | ImageUsage_ShaderResource;
				for (const auto& [format, size] : _colorAttachmentFormats)
				{
					_colorAttachments[index++] = Image::Create({width, height, usage, format, size});
				}

				_nativeRenderTargetViews.resize(inst->_colorAttachments.size());
				for (int i = 0; i < _colorAttachments.size(); i++)
				{
					auto* rtv = &_colorAttachments[i]->RenderViewAs<ID3D11RenderTargetView>();
					_nativeRenderTargetViews[i] = rtv;
				}
			}

			if (_depthStencilAttachmentFormat.Format != ImageFormat::None)
			{
				const auto& el = _depthStencilAttachmentFormat;
				_depthStencilAttachment = Image::Create(
					{width, height, ImageUsage_DepthStencil | ImageUsage_ShaderResource, el.Format, el.ArraySize}
				);
			}

			_width = width;
			_height = height;
			Resized.Invoke({_width, _height});
		}
	);
}

void Framebuffer::Clear()
{
	const auto inst = ShareThisAs<Framebuffer>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			for (auto& view : inst->_nativeRenderTargetViews)
			{
				package.Context.ClearRenderTargetView(view, reinterpret_cast<const float*>(&inst->_clearColor));
			}

			if (inst->_depthStencilAttachmentFormat.Format != ImageFormat::None)
			{
				package.Context.ClearDepthStencilView(
					&inst->_depthStencilAttachment->RenderViewAs<ID3D11DepthStencilView>(),
					D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
					inst->_depthClearValue,
					inst->_stencilClearValue
				);
			}
		}
	);
}

auto Framebuffer::Width() const -> uint
{
	return _width;
}

auto Framebuffer::Height() const -> uint
{
	return _height;
}

auto Framebuffer::ImageByIndex(uint index) -> Image&
{
	return *ImagePtrByIndex(index);
}

auto Framebuffer::ImageByIndex(uint index) const -> const Image&
{
	return *ImagePtrByIndex(index);
}

auto Framebuffer::ImagePtrByIndex(uint index) const -> const std::shared_ptr<Image>&
{
	Debug::Assert(index < _colorAttachments.size(), "Target index out of bounds");
	return _colorAttachments[index];
}

auto Framebuffer::DepthImage() -> Image&
{
	return *DepthImagePtr();
}

auto Framebuffer::DepthImage() const -> const Image&
{
	return *DepthImagePtr();
}

auto Framebuffer::DepthImagePtr() const -> const std::shared_ptr<Image>&
{
	Debug::Assert(_depthStencilAttachmentFormat.Format != ImageFormat::None, "No depth attachment in framebuffer");
	return _depthStencilAttachment;
}

void Framebuffer::SetDepthClearValue(float value)
{
	_depthClearValue = value;
}

auto Framebuffer::Create(const FramebufferSpec& spec) -> std::shared_ptr<Framebuffer>
{
	return BindableStore::Add<Framebuffer>(spec);
}
}
