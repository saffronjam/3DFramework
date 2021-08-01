#include "SaffronPCH.h"

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Saffron/Common/App.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/ErrorHandling/HrException.h"
#include "Saffron/Rendering/ErrorHandling/DxgiInfoException.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"


#include "Saffron/Rendering/Bindables/InputLayout.h"

namespace Se
{
Renderer::Renderer(const Window& window) :
	SingleTon(this),
	_bindableStore(std::make_unique<BindableStore>())
{
	constexpr auto debug = [] { return Configuration == AppConfiguration::Debug; }();

	CreateDeviceAndContext();
	CreateFactory();
	CreateSwapChain(window);

	_backbuffer = BackBuffer::Create(window);

	if constexpr (ConfDebug)
	{
		_dxgiInfoQueue = std::make_unique<DxgiInfoManager>();
	}
}

void Renderer::Execute()
{
	if constexpr (ConfDebug)
	{
		//Log::Info("Executing {} submitions", _submitions.Size());
	}

	auto package = RendererPackage{*_device.Get(), *_swapChain.Get(), *_context.Get()};

	std::swap(_submitingContainer, _executingContainer);
	for (auto& submition : *_executingContainer)
	{
		try
		{
			if constexpr (ConfDebug)
			{
				_dxgiInfoQueue->Begin();

				try
				{
					submition.Fn(package);
				}
				catch (const SaffronException& e)
				{
					auto result = _dxgiInfoQueue->End();
					if (result.size() > 0)
					{
						// Prioritize DxgiInfoException if it exists
						throw DxgiInfoException(std::move(result), submition.Location);
					}
					throw;
				}
				auto result = _dxgiInfoQueue->End();
				if (result.size() > 0)
				{
					throw DxgiInfoException(std::move(result), submition.Location);
				}
			}
			else
			{
				submition.Fn(package);
			}
		}
		catch (const HrException& e)
		{
			MessageBoxA(
				static_cast<HWND>(App::Instance().Window().NativeHandle()),
				e.What().c_str(),
				e.Type(),
				MB_OK | MB_ICONEXCLAMATION
			);
		} catch (const SaffronException& e)
		{
			MessageBoxA(
				static_cast<HWND>(App::Instance().Window().NativeHandle()),
				e.What().c_str(),
				e.Type(),
				MB_OK | MB_ICONEXCLAMATION
			);
		}
	}
	_executingContainer->clear();
}

void Renderer::BeginFrame()
{
	_backbuffer->Bind();
	_backbuffer->Clear();
}

void Renderer::EndFrame()
{
	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			package.SwapChain.Present(1, 0);
		}
	);

	Execute();
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

auto Renderer::BackBuffer() -> Se::BackBuffer&
{
	return *Instance()._backbuffer;
}

auto Renderer::BackBufferPtr() -> const std::shared_ptr<class BackBuffer>&
{
	return Instance()._backbuffer;
}

void Renderer::CleanDebugInfo()
{
	_dxgiInfoQueue->Begin();
}

void Renderer::CreateDeviceAndContext()
{
	uint swapCreateFlags = 0;

	if constexpr (ConfDebug)
	{
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	const auto hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&_device,
		nullptr,
		&_context
	);
	ThrowIfBad(hr);
}

void Renderer::CreateFactory()
{
	const auto hr = CreateDXGIFactory2(0, __uuidof(_factory), &_factory);
	ThrowIfBad(hr);
}

void Renderer::CreateSwapChain(const Window& window)
{
	DXGI_SWAP_CHAIN_DESC1 sd = {0};
	sd.BufferCount = 2;
	sd.Width = window.Width();
	sd.Height = window.Height();
	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.Scaling = DXGI_SCALING_NONE;
	sd.Stereo = false;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = 0;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd = {0};
	fd.RefreshRate.Numerator = 0;
	fd.RefreshRate.Denominator = 0;
	fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	fd.Windowed = true;

	const auto hr = _factory->CreateSwapChainForHwnd(
		_device.Get(),
		static_cast<HWND>(window.NativeHandle()),
		&sd,
		&fd,
		nullptr,
		&_swapChain
	);
	ThrowIfBad(hr);
}
}
