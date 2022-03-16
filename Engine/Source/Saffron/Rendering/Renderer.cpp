#include "SaffronPCH.h"

#include "Saffron/Rendering/Renderer.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <Windows.h>

#include "Saffron/Common/App.h"
#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Rendering/ErrorHandling/DxgiInfoException.h"
#include "Saffron/Rendering/ErrorHandling/HrException.h"
#include "Saffron/Rendering/VertexTypes.h"

namespace Se
{
Renderer::Renderer(const Window& window) :
	Singleton(this),
	_bindableStore(std::make_unique<BindableStore>()),
	_shaderStore(std::make_unique<ShaderStore>())
{
	BeginQueue("Main");

	CreateDeviceAndContext();
	CreateFactory();
	CreateSwapChain(window);

	_meshStore = std::make_unique<ModelStore>();
	_quadIndexBuffer = IndexBuffer::Create(4);
	_quadVertexBuffer = VertexBuffer::Create(PosColTexVertex::Layout(), 4);

	constexpr uint data = 0xffffffff;
	_whiteTexture = Texture::Create(1, 1, ImageFormat::RGBA, reinterpret_cast<const uchar*>(&data));

	_backbuffer = BackBuffer::Create(window);

	if constexpr (ConfDebug)
	{
		_dxgiInfoQueue = std::make_unique<DxgiInfoManager>();
	}
}

Renderer::~Renderer()
{
	EndQueue();
}

auto Renderer::Config() -> RendererConfig&
{
	return Instance()._config;
}

void Renderer::Submit(const RenderFn& fn, std::source_location location)
{
	if (auto& inst = Instance(); !inst._activeContainer->Executing && inst._activeStrategy == RenderStrategy::Deferred)
	[[likely]]
	{
		inst._activeContainer->Submitions.emplace_back(Submition{fn, location});
	}
	else
	{
		inst.ExecuteSubmition(Submition{fn, location});
	}
}

void Renderer::Execute()
{
	if (_activeContainer == nullptr)
	{
		throw SaffronException("Call to Execute with out am active render queue");
	}

	auto* executingContainer = _activeContainer;

	executingContainer->Executing = true;
	for (auto& submition : executingContainer->Submitions)
	{
		ExecuteSubmition(submition);
	}
	executingContainer->Executing = false;
	executingContainer->Submitions.clear();
}

void Renderer::BeginFrame()
{
	_backbuffer->Bind();
	_backbuffer->Clear();
	SetRenderState(RenderState::Default);
}

void Renderer::EndFrame()
{
	Submit(
		[this](const RendererPackage& package)
		{
			package.SwapChain.Present(1, 0);
		}
	);
}

void Renderer::BeginQueue(const std::string name)
{
	_queues.push(name);
	const auto findResult = _submitionContainers.find(name);
	if (findResult == _submitionContainers.end())
	{
		const auto result = _submitionContainers.emplace(name, RenderQueue{name});
		_activeContainer = &result.first->second;
	}
	else
	{
		_activeContainer = &findResult->second;
	}
}

void Renderer::EndQueue()
{
	Debug::Assert(!_activeContainer->Executing, "Can't end a render queue if the active queue is executing");

	if (_queues.size() == 0)
	{
		throw SaffronException("No queue to end.");
	}
	_queues.pop();

	if (_queues.size() == 0)
	{
		_activeContainer = nullptr;
	}
	else
	{
		const auto findResult = _submitionContainers.find(_queues.top());
		Debug::Assert(findResult != _submitionContainers.end());
		_activeContainer = &findResult->second;
	}
}

void Renderer::BeginStrategy(RenderStrategy strategy)
{
	auto& inst = Instance();
	inst._strategies.push(strategy);
	inst._activeStrategy = strategy;
}

void Renderer::EndStrategy()
{
	auto& inst = Instance();
	if (inst._strategies.size() == 0)
	{
		throw SaffronException("No strategy to end.");
	}
	inst._strategies.pop();

	if (inst._strategies.size() == 0)
	{
		inst._activeStrategy = RenderStrategy::Deferred;
	}
	else
	{
		inst._activeStrategy = inst._strategies.top();
	}
}

void Renderer::SubmitIndexed(uint indexCount, uint baseIndex, uint baseVertex)
{
	Submit(
		[=](const RendererPackage& package)
		{
			package.Context.DrawIndexed(indexCount, baseIndex, baseVertex);
		}
	);
}

void Renderer::SubmitFullscreenQuad()
{
	Submit(
		[](const RendererPackage& package)
		{
			const auto& inst = Instance();
			inst._quadIndexBuffer->Bind();
			inst._quadVertexBuffer->Bind();
			package.Context.DrawIndexed(4, 0, 0);
		}
	);
}

void Renderer::Log(const std::string& string)
{
	Submit(
		[string](const RendererPackage& package)
		{
			{
				std::cout << "[Renderer] " << string << '\n';
			}
		}
	);
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
	Submit(
		[state](const RendererPackage& package)
		{
			auto& inst = Instance();
			if (state != inst._submittedState)
			{
				inst.CreateRenderState(state);

				package.Context.OMSetDepthStencilState(inst._nativeDepthStencilState.Get(), 1);
				package.Context.RSSetState(inst._nativeRasterizerState.Get());
				package.Context.IASetPrimitiveTopology(inst._topology);

				inst._submittedState = state;
			}
		}
	);
}

void Renderer::ResetRenderState()
{
	SetRenderState(RenderState::Default);
}

void Renderer::SetViewportSize(uint width, uint height, float depth)
{
	Submit(
		[=](const RendererPackage& package)
		{
			const D3D11_VIEWPORT viewport{
				0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, depth
			};
			package.Context.RSSetViewports(1, &viewport);
		}
	);
}

auto Renderer::WhiteTexture() -> const std::shared_ptr<Texture>&
{
	return Instance()._whiteTexture;
}

void Renderer::CleanDebugInfo()
{
	_dxgiInfoQueue->Begin();
}

void Renderer::ExecuteSubmition(const Submition& submition)
{
	const auto package = RendererPackage{*_device.Get(), *_swapChain.Get(), *_context.Get()};

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

void Renderer::CreateRenderState(uint state)
{
	// Depth stencil

	D3D11_DEPTH_STENCIL_DESC dsd = {};

	if (state & RenderState::DepthTest_Less || state & RenderState::DepthTest_LessEqual || state &
		RenderState::DepthTest_Equal || state & RenderState::DepthTest_GreterEqual || state &
		RenderState::DepthTest_Greter || state & RenderState::DepthTest_NotEqual || state & RenderState::DepthTest_Never
		|| state & RenderState::DepthTest_Always)
	{
		dsd.DepthEnable = true;
		dsd.DepthFunc = Utils::ToD3D11CompFunc(state);
	}
	else
	{
		dsd.DepthEnable = false;
	}

	dsd.DepthWriteMask = Utils::ToD3D11WriteMask(state);
	auto hr = _device->CreateDepthStencilState(&dsd, &_nativeDepthStencilState);
	ThrowIfBad(hr);


	// Rasterizer

	D3D11_RASTERIZER_DESC rd = {};

	if (state & RenderState::Rasterizer_CullBack)
	{
		rd.CullMode = D3D11_CULL_BACK;
	}
	else if (state & RenderState::Rasterizer_CullFront)
	{
		rd.CullMode = D3D11_CULL_FRONT;
	}
	else
	{
		rd.CullMode = D3D11_CULL_NONE;
	}

	if (state & RenderState::Rasterizer_Wireframe)
	{
		rd.FillMode = D3D11_FILL_WIREFRAME;
	}
	else
	{
		rd.FillMode = D3D11_FILL_SOLID;
	}
	rd.DepthBias = 0;
	rd.SlopeScaledDepthBias = 2.0f;
	rd.DepthBiasClamp = 1.0f;

	hr = _device->CreateRasterizerState(&rd, &_nativeRasterizerState);
	ThrowIfBad(hr);

	// Topology
	_topology = Utils::ToD3D11PrimiteTopology(state);
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

D3D11_DEPTH_WRITE_MASK ToD3D11WriteMask(ulong state)
{
	if (state & RenderState::DepthWriteMask_All) return D3D11_DEPTH_WRITE_MASK_ALL;
	if (state & RenderState::DepthWriteMask_Zero) return D3D11_DEPTH_WRITE_MASK_ZERO;

	throw SaffronException("Invalid state. Could not convert to D3D11_DEPTH_WRITE_MASK.");
}

D3D11_PRIMITIVE_TOPOLOGY ToD3D11PrimiteTopology(ulong state)
{
	if (state & RenderState::Topology_TriangleList) return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	if (state & RenderState::Topology_LineList) return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	throw SaffronException("Invalid state. Could not convert to D3D11_PRIMITIVE_TOPOLOGY.");
}
}
}
