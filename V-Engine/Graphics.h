#pragma once

#include "VeWin.h"
#include <d3d11.h>

namespace ve
{
class Graphics
{
public:
	Graphics() = default;
	Graphics(const Graphics &) = delete;
	Graphics &operator=(const Graphics &) = delete;
	~Graphics();

	void Init(HWND hWnd);

	void EndFrame();
	void ClearBuffer(float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f) noexcept;
private:
	bool m_initialized;
private:
	ID3D11Device *m_pDevice = nullptr;
	IDXGISwapChain *m_pSwap = nullptr;
	ID3D11DeviceContext *m_pContext = nullptr;
	ID3D11RenderTargetView *m_pTarget = nullptr;
};
}

