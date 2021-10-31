#pragma once

#include <d3d11_4.h>
#include <source_location>
#include <vector>
#include <unordered_map>
#include <stack>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Graphics/ModelStore.h"
#include "Saffron/Rendering/BindableStore.h"
#include "Saffron/Rendering/ShaderStore.h"
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

enum class RenderStrategy
{
	Immediate,
	Deferred
};

using RenderFn = std::function<void(const RendererPackage& package)>;


struct Submition
{
	RenderFn Fn;
	std::source_location Location;
};

struct RenderQueue
{
	std::string Name;
	std::vector<Submition> Submitions;
	bool Executing = false;
};


class Model;
struct Mvp;
class TransformCBuffer;

class VertexBuffer;
class IndexBuffer;
class InputLayout;

class Renderer : public Singleton<Renderer>
{
public:
	explicit Renderer(const Window& window);
	~Renderer() override;

	static void Submit(const RenderFn& fn, std::source_location location = std::source_location::current());

	void Execute();

	void BeginFrame();
	void EndFrame();

	void BeginQueue(const std::string name);
	void EndQueue();

	static void BeginStrategy(RenderStrategy strategy);
	static void EndStrategy();

	static void SubmitIndexed(uint indexCount, uint baseIndex = 0, uint baseVertex = 0);
	static void SubmitFullscreenQuad();

	template <typename ... Args>
	static void Log(const std::string& string, const Args& ... args);
	static void Log(const std::string& string);

	static auto Device() -> ID3D11Device&;
	static auto Context() -> ID3D11DeviceContext&;
	static auto SwapChain() -> IDXGISwapChain&;
	static auto BackBuffer() -> BackBuffer&;
	static auto BackBufferPtr() -> const std::shared_ptr<class BackBuffer>&;

	static void SetRenderState(RenderState state);
	static void SetViewportSize(uint width, uint height);

	static auto WhiteTexture() -> const std::shared_ptr<Texture>&;

	void CleanDebugInfo();

private:
	void ExecuteSubmition(const Submition& submition);

private:
	void CreateDeviceAndContext();
	void CreateFactory();
	void CreateSwapChain(const Window& window);
	void CreateRenderState(uint state);

private:
	ComPtr<ID3D11Device> _device{};
	ComPtr<IDXGISwapChain1> _swapChain{};
	ComPtr<ID3D11DeviceContext> _context{};
	ComPtr<IDXGIFactory2> _factory{};
	
	// Render state
	RenderState _submittedState = 0;
	ComPtr<ID3D11DepthStencilState> _nativeDepthStencilState;
	ComPtr<ID3D11RasterizerState> _nativeRasterizerState;
	ComPtr<ID3D11SamplerState> _nativeSamplerState;
	D3D11_PRIMITIVE_TOPOLOGY _topology;

	// Renderer data
	std::shared_ptr<class BackBuffer> _backbuffer;
	std::unique_ptr<BindableStore> _bindableStore;
	std::unique_ptr<ShaderStore> _shaderStore;
	std::unique_ptr<ModelStore> _meshStore;
	std::shared_ptr<RenderGraph> _currentRenderGraph;

	// Prepared submitions
	std::shared_ptr<VertexBuffer> _quadVertexBuffer;
	std::shared_ptr<IndexBuffer> _quadIndexBuffer;

	// Submitions
	std::map<std::string, RenderQueue> _submitionContainers;
	RenderQueue* _activeContainer = nullptr;
	std::stack<std::string> _queues;

	// Strategy
	RenderStrategy _activeStrategy = RenderStrategy::Deferred;
	std::stack<RenderStrategy> _strategies;

	// Prepare textures
	std::shared_ptr<Texture> _whiteTexture;

	// Only initialized in debug
	std::unique_ptr<DxgiInfoManager> _dxgiInfoQueue{};
};

template <typename ... Args>
void Renderer::Log(const std::string& string, const Args&... args)
{
	Log(std::format(string, args...));
}

namespace Utils
{
D3D11_COMPARISON_FUNC ToD3D11CompFunc(ulong state);
D3D11_PRIMITIVE_TOPOLOGY ToD3D11PrimiteTopology(ulong state);
}
}
