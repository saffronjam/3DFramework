#pragma once

#include <d3d11_4.h>
#include <source_location>
#include <vector>
#include <unordered_map>
#include <stack>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Graphics/MeshStore.h"
#include "Saffron/Graphics/Shapes/Rect.h"
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


class Mesh;
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

	static void Submit(const RenderFn& fn, std::source_location location = std::source_location::current())
	{
		if (auto& inst = Instance(); !inst._activeContainer->Executing && inst._strategy == RenderStrategy::Deferred)
		[[likely]]
		{
			inst._activeContainer->Submitions.emplace_back(Submition{fn, location});
		}
		else
		{
			inst.ExecuteSubmition(Submition{fn, location});
		}
	}

	void Execute();

	void BeginFrame();
	void EndFrame();

	void BeginQueue(const std::string name);
	void EndQueue();

	static void SubmitIndexed();
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
	static void SetRenderStrategy(RenderStrategy strategy);
	static void SetViewportSize(uint width, uint height);

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

	RenderStrategy _strategy = RenderStrategy::Deferred;

	// Render state
	RenderState _submittedState = 0;
	ComPtr<ID3D11DepthStencilState> _nativeDepthStencilState;
	ComPtr<ID3D11RasterizerState> _nativeRasterizerState;
	ComPtr<ID3D11SamplerState> _nativeSamplerState;
	D3D11_PRIMITIVE_TOPOLOGY _topology;

	// Renderer data
	std::shared_ptr<class BackBuffer> _backbuffer;
	std::unique_ptr<BindableStore> _bindableStore;
	std::unique_ptr<MeshStore> _meshStore;
	std::shared_ptr<RenderGraph> _currentRenderGraph;

	// Prepared submitions
	std::shared_ptr<VertexBuffer> _quadVertexBuffer;
	std::shared_ptr<IndexBuffer> _quadIndexBuffer;
	std::shared_ptr<InputLayout> _quadInputLayout;

	// Submitions
	std::map<std::string, RenderQueue> _submitionContainers;
	RenderQueue* _activeContainer = nullptr;
	std::stack<std::string> _queues;

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
