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
	CreateMainTarget(window);

	if constexpr (ConfDebug)
	{
		_dxgiInfoQueue = std::make_unique<DxgiInfoManager>();
	}

	struct Vertex
	{
		float x, y;
	};

	const VertexLayout layout({ElementType::Position2D});

	VertexStorage storage(layout);

	storage.Add(Vertex{0.0f, 0.5f});
	storage.Add(Vertex{0.5f, -0.5f});
	storage.Add(Vertex{-0.5f, -0.5f});

	_framebuffer = Framebuffer::Create({window.Width(), window.Height(), {ImageFormat::RGBA}});
	_vertexBuffer = VertexBuffer::Create(storage);
	_vertexShader = VertexShader::Create("VertexShader_v");
	_pixelShader = PixelShader::Create("PixelShader_p");
	_layout = InputLayout::Create(layout, _vertexShader);
}

void Renderer::Execute()
{
	if constexpr (ConfDebug)
	{
		//Log::Info("Executing {} submitions", _submitions.size());
	}

	std::swap(_submitingContainer, _executingContainer);
	_submitingContainer->clear();

	auto package = RendererPackage{*_device.Get(), *_swapChain.Get(), *_context.Get()};

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
	_vertexBuffer->Bind();
	_vertexShader->Bind();
	_pixelShader->Bind();
	_layout->Bind();
	_framebuffer->Bind();

	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			// Specify output target
			//_context->OMSetRenderTargets(1u, _mainTarget.GetAddressOf(), nullptr);

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			ComPtr<ID3D11RenderTargetView> renderTargetView;

			auto& nonConst = const_cast<ID3D11Texture2D&>(tex->NativeTexture());
			auto hr = package.Device.CreateRenderTargetView(&nonConst, &renderTargetViewDesc, &renderTargetView);

			/*tex->Bind();

			package.Context.OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);*/

			ImGui::Begin("Image");
			ImGui::Image((void*)(&tex->NativeShaderResourceView()), {100, 100});
			ImGui::End();


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


			_context->Draw(static_cast<UINT>(_vertexBuffer->VertexCount()), 0);
		}
	);

	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			// Specify output target
			_context->OMSetRenderTargets(1u, _mainTarget.GetAddressOf(), nullptr);

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			ComPtr<ID3D11RenderTargetView> renderTargetView;

			auto& nonConst = const_cast<ID3D11Texture2D&>(tex->NativeTexture());
			auto hr = package.Device.CreateRenderTargetView(&nonConst, &renderTargetViewDesc, &renderTargetView);

			/*tex->Bind();

			package.Context.OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);*/

			ImGui::Begin("Image");
			ImGui::Image((void*)(&_framebuffer->Target().ShaderView()), { 100, 100 });
			ImGui::End();


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
			
		}
	);
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
	D3D11_VIEWPORT vp{0.0f, 0.0f, window.Width(), window.Height(), 0.0f, 0.0f,};

	_context->RSSetViewports(1, &vp);

	ComPtr<ID3D11Resource> backBuffer;
	auto hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);
	ThrowIfBad(hr);

	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, &_mainTarget);
	ThrowIfBad(hr);
}
}
