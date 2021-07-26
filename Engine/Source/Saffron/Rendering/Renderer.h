#pragma once

#include <source_location>
#include <d3d11.h>

#include "ErrorHandling/HrException.h"
#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"

namespace Se
{
using HRESULT = long;
using RenderFn = std::function<void()>;

class Renderer : public SingleTon<Renderer>
{
public:
	explicit Renderer(const Window& window);

	static void Submit(RenderFn fn)
	{
		Instance()._submitions.emplace_back(std::move(fn));
	}

	void Execute();

	static void ThrowIfBad(
		HRESULT hResult,
		const std::source_location location = std::source_location::current()
	) noexcept(!ConfDebug)
	{
		if (BadHResult(hResult))
		{
			if (hResult != DXGI_ERROR_DEVICE_REMOVED)
			{
				throw HrException(hResult, location);
			}
			throw HrException(Instance()._device->GetDeviceRemovedReason(), location);
		}
	}

	static auto Device() -> ID3D11Device&;
	static auto Context()->ID3D11DeviceContext&;
	static auto SwapChain() -> IDXGISwapChain&;

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D11DeviceContext> _context;

	std::vector<RenderFn> _submitions;
};
}
