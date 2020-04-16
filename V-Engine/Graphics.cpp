#include "Graphics.h"

ve::Graphics::~Graphics()
{
	if ( !m_initialized )
		return;

	if ( m_pContext != nullptr )
		m_pContext->Release();
	if ( m_pSwap != nullptr )
		m_pSwap->Release();
	if ( m_pDevice != nullptr )
		m_pDevice->Release();
	if ( m_pTarget != nullptr )
		m_pTarget->Release();
}

void ve::Graphics::Init(HWND hWnd)
{
	if ( m_initialized )
		return;

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwap,
		&m_pDevice,
		nullptr,
		&m_pContext
	);

	ID3D11Resource *pBackBuffer = nullptr;
	m_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void **>(&pBackBuffer));

	m_pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&m_pTarget
	);

	m_initialized = true;
}

void ve::Graphics::EndFrame()
{
	m_pSwap->Present(1u, 0u);
}

void ve::Graphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	m_pContext->ClearRenderTargetView(m_pTarget, color);
}
