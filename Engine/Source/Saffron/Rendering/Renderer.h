#pragma once

#include <d3d11_4.h>
#include <source_location>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Graphics/Mesh.h"
#include "Saffron/Graphics/MeshStore.h"
#include "Saffron/Rendering/BindableStore.h"
#include "Saffron/Rendering/RenderGraph.h"
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

class Renderer : public SingleTon<Renderer>
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
	static void Submit(RenderFn fn, std::source_location location = std::source_location::current())
	{
		Instance()._submitingContainer->emplace_back(Submition{std::move(fn), location});
	}
#else
	static void Submit(RenderFn fn)
	{
		Instance()._submitingContainer->.emplace_back(std::move(fn));
	}
#endif
	static void SubmitMesh(const std::shared_ptr<Mesh>& mesh)
	{
		Renderer::Submit(
			[mesh](const RendererPackage& package)
			{
				for (const auto& submesh : mesh->SubMeshes())
				{
					package.Context.DrawIndexed(submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
				};
			}
		);
	}

	void Execute();

	void BeginFrame();
	void EndFrame();

	static auto Device() -> ID3D11Device&;
	static auto Context() -> ID3D11DeviceContext&;
	static auto SwapChain() -> IDXGISwapChain&;
	static auto BackBuffer() -> BackBuffer&;
	static auto BackBufferPtr() -> const std::shared_ptr<class BackBuffer>&;

	void CleanDebugInfo();

private:
	void CreateDeviceAndContext();
	void CreateFactory();
	void CreateSwapChain(const Window& window);

private:
	ComPtr<ID3D11Device> _device{};
	ComPtr<IDXGISwapChain1> _swapChain{};
	ComPtr<ID3D11DeviceContext> _context{};
	ComPtr<IDXGIFactory2> _factory{};

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
}
