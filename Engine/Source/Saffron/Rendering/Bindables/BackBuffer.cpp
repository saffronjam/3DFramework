#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/BackBuffer.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
BackBuffer::BackBuffer(const Window& window)
{
	SetInitializer(
		[this, &window]
		{
			const auto inst = ShareThisAs<BackBuffer>();
			Renderer::Submit(
				[inst, &window](const RendererPackage& package)
				{
					window.Resized += [inst]<typename SizeEvent>(SizeEvent&& sizeEvent)
					{
						inst->OnWindowResize(std::forward<SizeEvent>(sizeEvent));
						return false;
					};

					auto mainImage = Image::CreateFromBackBuffer();

					inst->_colorAttachmentFormats.push_back(mainImage->Format());
					inst->_colorAttachments.push_back(std::move(mainImage));
				}
			);
			Resize(window.Width(), window.Height());
		}
	);
}

void BackBuffer::Resize(uint width, uint height)
{
	const auto inst = ShareThisAs<BackBuffer>();
	Renderer::Submit(
		[inst, width, height](const RendererPackage& package)
		{
			if (width == inst->_width && height == inst->_height)
			{
				// Already resized
				return;
			}

			inst->_width = width;
			inst->_height = height;

			inst->_colorAttachments.clear();
			inst->_nativeRenderTargetViews.clear();

			auto backBuffer = Image::CreateFromBackBuffer();

			inst->_colorAttachments.push_back(backBuffer);
			inst->_nativeRenderTargetViews.push_back(&backBuffer->RenderViewAs<ID3D11RenderTargetView>());
		}
	);
}

auto BackBuffer::Create(const Window& window) -> std::shared_ptr<BackBuffer>
{
	return BindableStore::Add<BackBuffer>(window);
}

void BackBuffer::OnWindowResize(const SizeEvent& event)
{
	if (event.Width == _width && event.Height == _height)
	{
		// No need to resize
		return;
	}

	Resize(event.Width, event.Height);
}
}
