#include "SaffronPCH.h"

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
						[&inst](ImageFormat format)
						{
							return Utils::IsDepthFormat(format);
						}
					);

					if (iter != inst->_colorAttachmentFormats.end())
					{
						inst->_depthStencilAttachmentFormat = *iter;
					}

					std::erase_if(
						inst->_colorAttachmentFormats,
						[](ImageFormat format)
						{
							return Utils::IsDepthFormat(format);
						}
					);
				}
			);
			Resize(width, height);
		}
	);
}

void Framebuffer::Bind()
{
	const auto inst = ShareThisAs<Framebuffer>();
	Renderer::Submit(
		[inst](const RendererPackage& package)
		{
			auto* ds = inst->_depthStencilAttachmentFormat == ImageFormat::None
				           ? nullptr
				           : &inst->_depthStencilAttachment->RenderViewAs<ID3D11DepthStencilView>();
			auto* const* rs = inst->_colorAttachments.empty() ? nullptr : inst->_nativeRenderTargetViews.data();
			package.Context.OMSetRenderTargets(inst->_nativeRenderTargetViews.size(), rs, ds);
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
				const auto usage = ImageUsage_RenderTarget | ImageUsage_ShaderResource;
				for (const auto format : _colorAttachmentFormats)
				{
					_colorAttachments[index++] = Image::Create({width, height, usage, format});
				}

				_nativeRenderTargetViews.resize(inst->_colorAttachments.size());
				for (int i = 0; i < _colorAttachments.size(); i++)
				{
					auto* rtv = &_colorAttachments[i]->RenderViewAs<ID3D11RenderTargetView>();
					_nativeRenderTargetViews[i] = rtv;
				}
			}

			if (_depthStencilAttachmentFormat != ImageFormat::None)
			{
				_depthStencilAttachment = Image::Create(
					{width, height, ImageUsage_DepthStencil | ImageUsage_ShaderResource, _depthStencilAttachmentFormat}
				);
			}

			_width = width;
			_height = height;
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

			if (inst->_depthStencilAttachmentFormat != ImageFormat::None)
			{
				// TODO: Clear depth attachment
				/*package.Context.ClearDepthStencilView(
					&inst->_depthStencilAttachment->RenderViewAs<ID3D11DepthStencilView>(),
				);*/
			}
		}
	);
}

auto Framebuffer::TargetByIndex(uint index) const -> const Image&
{
	Debug::Assert(index < _colorAttachments.size(), "Target index out of bounds");
	return *_colorAttachments[index];
}

const Image& Framebuffer::FinalTarget() const
{
	return *_colorAttachments.front();
}

auto Framebuffer::DepthTarget() const -> const Image&
{
	Debug::Assert(_depthStencilAttachmentFormat != ImageFormat::None, "No depth attachment in framebuffer");
	return *_depthStencilAttachment;
}

auto Framebuffer::Create(const FramebufferSpec& spec) -> std::shared_ptr<Framebuffer>
{
	return BindableStore::Add<Framebuffer>(spec);
}
}
