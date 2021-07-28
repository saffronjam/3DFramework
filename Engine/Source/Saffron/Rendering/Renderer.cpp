#include "SaffronPCH.h"

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Saffron/Common/App.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/ErrorHandling/HrException.h"
#include "Saffron/Rendering/ErrorHandling/DxgiInfoException.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"

namespace Se
{
Renderer::Renderer(const Window& window) :
	SingleTon(this)
{
	constexpr auto debug = [] { return Configuration == AppConfiguration::Debug; }();

	CreateDeviceAndContext();
	CreateFactory();
	CreateSwapChain(window);
	CreateMainTarget(window);
	
	if constexpr (ConfDebug)
	{
		_dxgiInfoQueue = std::make_unique<DxgiInfoManager>();
	}
}

void Renderer::Execute()
{
	if constexpr (ConfDebug)
	{
		//Log::Info("Executing {} submitions", _submitions.size());
	}

	for (auto& submition : _submitions)
	{
		try
		{
			if constexpr (ConfDebug)
			{
				_dxgiInfoQueue->Begin();
				submition.Fn();
				const auto result = _dxgiInfoQueue->End();
				if (result.size() > 0)
				{
					throw DxgiInfoException(std::move(result), submition.Location);
				}
			}
			else
			{
				submition.Fn();
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

auto Renderer::Target() -> ID3D11RenderTargetView&
{
	return *Instance()._mainTarget.Get();
}

void Renderer::DrawTestTriangle()
{
	struct Vertex
	{
		float x, y;
	};

	const Vertex vertices[] = {{0.0f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5}};

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	ComPtr<ID3D11Buffer> vertexBuffer;
	HRESULT hr;
	ThrowIfBad(_device->CreateBuffer(&bd, &sd, &vertexBuffer));

	const uint stride = sizeof(Vertex);
	const auto offset = 0u;

	_context->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);

	ComPtr<ID3DBlob> blob;

	// Load Pixel shader and bind it
	ComPtr<ID3D11PixelShader> pixelShader;
	ThrowIfBad(D3DReadFileToBlob(L"Assets/Shaders/PixelShader_p.cso", &blob));
	ThrowIfBad(_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));


	_context->PSSetShader(pixelShader.Get(), nullptr, 0u);


	// Load Vertex shader and bind it
	ComPtr<ID3D11VertexShader> vertexShader;
	ThrowIfBad(D3DReadFileToBlob(L"Assets/Shaders/VertexShader_v.cso", &blob));

	ThrowIfBad(_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));

	_context->VSSetShader(vertexShader.Get(), nullptr, 0u);


	// Setup input layout nad bind it
	ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	ThrowIfBad(
		_device->CreateInputLayout(
			ied,
			static_cast<UINT>(std::size(ied)),
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			&inputLayout
		)
	);

	_context->IASetInputLayout(inputLayout.Get());

	// Specify output target
	_context->OMSetRenderTargets(1u, _mainTarget.GetAddressOf(), nullptr);


	const auto& window = App::Instance().Window();

	// Configure viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<FLOAT>(window.Width());
	vp.Height = static_cast<FLOAT>(window.Height());
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	_context->RSSetViewports(1u, &vp);

	// Set primitive topology
	_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	_dxgiInfoQueue->Begin();
	_context->Draw(static_cast<UINT>(std::size(vertices)), 0);
	const auto result = _dxgiInfoQueue->End();

	if (!result.empty())
	{
		throw SaffronException(result[0], std::source_location::current());
	}
	
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
	sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

void Renderer::CreateMainTarget(const Window& window)
{
	D3D11_VIEWPORT vp;
	vp.Width = window.Width();
	vp.Height = window.Height();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 0.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;

	_context->RSSetViewports(1, &vp);

	ComPtr<ID3D11Resource> backBuffer;
	auto hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);
	ThrowIfBad(hr);

	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, &_mainTarget);
	ThrowIfBad(hr);
}
}
