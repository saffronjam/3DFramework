#include "SaffronPCH.h"

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/RendererApi.h"

namespace Se
{
void RendererApi::Clear(const Vector4& color)
{
	Renderer::Context().ClearRenderTargetView(&Renderer::Target(), reinterpret_cast<const FLOAT*>(&color));
}

void RendererApi::Present()
{
	const auto hr = Renderer::SwapChain().Present(1u, 0);
	Renderer::ThrowIfBad(hr);
}
}
