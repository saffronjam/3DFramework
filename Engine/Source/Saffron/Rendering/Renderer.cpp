#include "SaffronPCH.h"

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Saffron/Common/App.h"
#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/ErrorHandling/HrException.h"
#include "Saffron/Rendering/ErrorHandling/DxgiInfoException.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"

namespace Se
{
Renderer::Renderer(const Window& window) :
	Singleton(this),
	_bindableStore(std::make_unique<BindableStore>()),
	_meshStore(std::make_unique<MeshStore>())
{
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
	if (_requestedState != _submittedState)
	{
		CreateRenderState();
		_submittedState = _requestedState;
	}

	_backbuffer->Bind();
	_backbuffer->Clear();

	Renderer::Submit(
		[](const RendererPackage& package)
		{
			auto& inst = Instance();

			package.Context.OMSetDepthStencilState(inst._nativeDepthStencilState.Get(), 1);
			package.Context.RSSetState(inst._nativeRasterizerState.Get());
			package.Context.PSSetSamplers(0, 1, inst._nativeSamplerState.GetAddressOf());
			package.Context.IASetPrimitiveTopology(inst._topology);
		}
	);
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

void Renderer::SetRenderState(RenderState state)
{
	Instance()._requestedState = state;
	Renderer::Submit(
		[](const RendererPackage& package)
		{
			auto& inst = Instance();
			if (inst._requestedState != inst._submittedState)
			{
				inst.CreateRenderState();
				inst._submittedState = inst._requestedState;
			}
		}
	);
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

void Renderer::CreateRenderState()
{
	// Depth stencil

	D3D11_DEPTH_STENCIL_DESC dsd = {};

	if (_requestedState & RenderState::DepthTest_Less || _requestedState & RenderState::DepthTest_LessEqual ||
		_requestedState & RenderState::DepthTest_Equal || _requestedState & RenderState::DepthTest_GreterEqual ||
		_requestedState & RenderState::DepthTest_Greter || _requestedState & RenderState::DepthTest_NotEqual ||
		_requestedState & RenderState::DepthTest_Never || _requestedState & RenderState::DepthTest_Always)
	{
		dsd.DepthEnable = true;
		dsd.DepthFunc = Utils::ToD3D11CompFunc(_requestedState);
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	}
	else
	{
		dsd.DepthEnable = false;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	auto hr = _device->CreateDepthStencilState(&dsd, &_nativeDepthStencilState);
	ThrowIfBad(hr);


	// Rasterizer

	D3D11_RASTERIZER_DESC rd = {};

	if (_requestedState & RenderState::Rasterizer_CullBack)
	{
		rd.CullMode = D3D11_CULL_BACK;
	}
	else if (_requestedState & RenderState::Rasterizer_CullBack)
	{
		rd.CullMode = D3D11_CULL_FRONT;
	}
	else
	{
		rd.CullMode = D3D11_CULL_NONE;
	}

	if (_requestedState & RenderState::Rasterizer_Wireframe)
	{
		rd.FillMode = D3D11_FILL_WIREFRAME;
	}
	else
	{
		rd.FillMode = D3D11_FILL_SOLID;
	}

	hr = _device->CreateRasterizerState(&rd, &_nativeRasterizerState);
	ThrowIfBad(hr);


	// Sampler

	D3D11_SAMPLER_DESC sd = {};

	sd.Filter = Utils::ToD3D11Filter(_requestedState);
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if (_requestedState & RenderState::Sampler_Anisotropic)
	{
		sd.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	}

	hr = _device->CreateSamplerState(&sd, &_nativeSamplerState);
	ThrowIfBad(hr);


	// Topology

	if (_requestedState & RenderState::Topology_TriangleList)
	{
		_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

namespace Utils
{
D3D11_COMPARISON_FUNC ToD3D11CompFunc(ulong state)
{
	if (state & RenderState::DepthTest_Less) return D3D11_COMPARISON_LESS;
	if (state & RenderState::DepthTest_LessEqual) return D3D11_COMPARISON_LESS_EQUAL;
	if (state & RenderState::DepthTest_Equal) return D3D11_COMPARISON_EQUAL;
	if (state & RenderState::DepthTest_GreterEqual) return D3D11_COMPARISON_GREATER_EQUAL;
	if (state & RenderState::DepthTest_Greter) return D3D11_COMPARISON_GREATER;
	if (state & RenderState::DepthTest_NotEqual) return D3D11_COMPARISON_NOT_EQUAL;
	if (state & RenderState::DepthTest_Never) return D3D11_COMPARISON_NEVER;
	if (state & RenderState::DepthTest_Always) return D3D11_COMPARISON_ALWAYS;

	throw SaffronException("Invalid state. Could not convert to D3D11_COMPARISON_FUNC.");
}

D3D11_FILTER ToD3D11Filter(ulong state)
{
	if (state & RenderState::Sampler_Anisotropic) return D3D11_FILTER_ANISOTROPIC;
	if (state & RenderState::Sampler_Bilinear) return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	if (state & RenderState::Sampler_Point) return D3D11_FILTER_MIN_MAG_MIP_POINT;

	throw SaffronException("Invalid filter. Could not convert to D3D11_FILTER.");
}
}
}
