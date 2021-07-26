#include "SaffronPCH.h"

#include <Windows.h>
#include <d3d11.h>

#include "Saffron/Common/App.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/ErrorHandling/HrException.h"

namespace Se
{
Renderer::Renderer(const Window& window) :
	SingleTon(this)
{
	constexpr auto debug = [] { return Configuration == AppConfiguration::Debug; }();

	uint swapCreateFlags = 0;

	if constexpr (debug)
	{
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}


	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = window.Width();
	sd.BufferDesc.Height = window.Height();
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = 0;
	sd.OutputWindow = static_cast<HWND>(window.NativeHandle());
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed = true;

	const auto result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&_swapChain,
		&_device,
		nullptr,
		&_context
	);

	Debug::Assert(GoodHResult(result), "Failed to create Device and SwapChain");

	D3D11_VIEWPORT vp;
	vp.Width = window.Width();
	vp.Height = window.Height();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 0.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;

	_context->RSSetViewports(1, &vp);
}

void Renderer::Execute()
{
	if constexpr (ConfDebug)
	{
		Log::Info("Executing {} submitions", _submitions.size());
	}

	for (auto& submition : _submitions)
	{
		try
		{
			submition();
		}
		catch (const HrException& e)
		{
			MessageBoxA(
				static_cast<HWND>(App::Instance().Window().NativeHandle()),
				e.Type(),
				e.What().c_str(),
				MB_OK | MB_ICONEXCLAMATION
			);
		}
	}
	_submitions.clear();
}

auto Renderer::Device() -> ID3D11Device&
{
	return *Instance()._device.Get();
}

auto Renderer::Context() -> ID3D11DeviceContext&
{
	return *Instance()._context.Get();
}

auto Renderer::SwapChain() -> IDXGISwapChain&
{
	return *Instance()._swapChain.Get();
}
}
