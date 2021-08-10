#pragma once

#include <d3d11_4.h>
#include <source_location>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Graphics/MeshStore.h"
#include "Saffron/Rendering/BindableStore.h"
#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/RenderState.h"
#include "Saffron/Rendering/Bindables/BackBuffer.h"
#include "Saffron/Rendering/ErrorHandling/DxgiInfoManager.h"

namespace Se
{
using HRESULT = long;

struct RendererPackage
{
	ID3D11Device& Device;
	IDXGISwapChain1& SwapChain;
	ID3D11DeviceContext& Context;
};

using RenderFn = std::function<void(const RendererPackage& package)>;

class Mesh;
struct Mvp;
class MvpCBuffer;

class Renderer : public Singleton<Renderer>
{
private:
	struct Submition
	{
		RenderFn Fn;
#ifdef SE_DEBUG
		std::source_location Location;
#endif
	};

public:
	explicit Renderer(const Window& window);

#ifdef SE_DEBUG
	static void Submit(const RenderFn& fn, std::source_location location = std::source_location::current())
	{
		Instance()._submitingContainer->emplace_back(Submition{fn, location});
	}
#else
	static void Submit(RenderFn fn)
	{
		Instance()._submitingContainer->.emplace_back(std::move(fn));
	}
#endif

	void Execute();

	void BeginFrame();
	void EndFrame();

	static auto Device() -> ID3D11Device&;
	static auto Context() -> ID3D11DeviceContext&;
	static auto SwapChain() -> IDXGISwapChain&;
	static auto BackBuffer() -> BackBuffer&;
	static auto BackBufferPtr() -> const std::shared_ptr<class BackBuffer>&;

	static void SetRenderState(RenderState state);

	void CleanDebugInfo();

private:
	void CreateDeviceAndContext();
	void CreateFactory();
	void CreateSwapChain(const Window& window);
	void CreateRenderState();

private:
	ComPtr<ID3D11Device> _device{};
	ComPtr<IDXGISwapChain1> _swapChain{};
	ComPtr<ID3D11DeviceContext> _context{};
	ComPtr<IDXGIFactory2> _factory{};

	// Render state
	RenderState _submittedState;
	RenderState _requestedState = RenderState::Default;
	ComPtr<ID3D11DepthStencilState> _nativeDepthStencilState;
	ComPtr<ID3D11RasterizerState> _nativeRasterizerState;
	ComPtr<ID3D11SamplerState> _nativeSamplerState;
	D3D11_PRIMITIVE_TOPOLOGY _topology;
	
	std::shared_ptr<class BackBuffer> _backbuffer;

	std::unique_ptr<BindableStore> _bindableStore;
	std::unique_ptr<MeshStore> _meshStore;
	std::shared_ptr<RenderGraph> _currentRenderGraph;

	// Submitions	
	std::vector<Submition> _submitionsPrimary;
	std::vector<Submition> _submitionsSecondary;
	std::vector<Submition>* _executingContainer = &_submitionsPrimary;
	std::vector<Submition>* _submitingContainer = &_submitionsSecondary;

	// Only initialized in debug
	std::unique_ptr<DxgiInfoManager> _dxgiInfoQueue{};
};

namespace Utils
{
D3D11_COMPARISON_FUNC ToD3D11CompFunc(ulong state);
D3D11_FILTER ToD3D11Filter(ulong state);
}
}
