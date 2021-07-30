#pragma once

#include <source_location>
#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Rendering/BindableStore.h"
#include "Saffron/Rendering/ErrorHandling/DxgiInfoManager.h"


#include "Bindables.h"

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

	void Execute();

	static auto Device() -> ID3D11Device&;
	static auto Context() -> ID3D11DeviceContext&;
	static auto SwapChain() -> IDXGISwapChain&;
	static auto Target() -> ID3D11RenderTargetView&;

	void DrawTestTriangle();

private:
	void CreateDeviceAndContext();
	void CreateFactory();
	void CreateSwapChain(const Window& window);
	void CreateMainTarget(const Window& window);

private:
	ComPtr<ID3D11Device> _device{};
	ComPtr<IDXGISwapChain1> _swapChain{};
	ComPtr<ID3D11DeviceContext> _context{};
	ComPtr<ID3D11RenderTargetView> _mainTarget{};
	ComPtr<IDXGIFactory2> _factory{};

	std::vector<Submition> _submitionsPrimary;
	std::vector<Submition> _submitionsSecondary;
	std::vector<Submition>* _executingContainer = &_submitionsPrimary;
	std::vector<Submition>* _submitingContainer = &_submitionsSecondary;

	std::unique_ptr<BindableStore> _bindableStore;

	// Only initialized in debug
	std::unique_ptr<DxgiInfoManager> _dxgiInfoQueue{};

	std::shared_ptr<Framebuffer> _framebuffer;
	std::shared_ptr<InputLayout> _layout;
	std::shared_ptr<VertexShader> _vertexShader;
	std::shared_ptr<PixelShader> _pixelShader;
	std::shared_ptr<VertexBuffer> _vertexBuffer;
	Viewport _viewport;
	PrimitiveTopology _topology = PrimitiveTopologyType::TriangleList;

	std::shared_ptr<Texture> tex = Texture::Create(100, 100);
};
}
