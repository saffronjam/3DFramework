#include "SaffronPCH.h"

#include <Dxgi1_2.h>

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
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
	const auto hr = Renderer::SwapChain().Present(1, 0);
	ThrowIfBad(hr);
}
}
