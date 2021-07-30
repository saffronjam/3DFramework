#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindables/Viewport.h"

#include "Saffron/Rendering/Renderer.h"

namespace Se
{
Viewport::Viewport(float width, float height) :
	_nativeViewport({0.0f, 0.0f, width, height, 0.0f, 1.0f})
{
}

void Viewport::Bind()
{
	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			package.Context.RSSetViewports(1, &_nativeViewport);
		}
	);
}

void Viewport::Resize(float width, float height)
{
	_nativeViewport.Width = width;
	_nativeViewport.Height = height;
}
}
