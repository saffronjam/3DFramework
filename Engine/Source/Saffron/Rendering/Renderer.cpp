#include "SaffronPCH.h"

#include <Windows.h>
#include <d3d11.h>

#include "Saffron/Common/App.h"
#include "Saffron/Rendering/Renderer.h"

#include <d3dcompiler.h>

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

	ID3D11Resource* backBuffer = nullptr;
	auto hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuffer));
	hr = _device->CreateRenderTargetView(backBuffer, nullptr, &_mainTarget);
	Debug::Assert(GoodHResult(hr), "Failed to create main RenderTargetView");
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
	ThrowIfBad(D3DReadFileToBlob(L"Shaders/PixelShader.pixel.cso", &blob));
	ThrowIfBad(_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));


	_context->PSSetShader(pixelShader.Get(), nullptr, 0u);


	// Load Vertex shader and bind it
	ComPtr<ID3D11VertexShader> vertexShader;
	ThrowIfBad(D3DReadFileToBlob(L"Shaders/VertexShader.vertex.cso", &blob));

	ThrowIfBad(_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));;;

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

	_context->Draw(static_cast<UINT>(std::size(vertices)), 0);
}
}
