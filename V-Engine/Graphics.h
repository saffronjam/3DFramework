#pragma once

#include "VeWin.h"
#include <d3d11.h>
#include <string>

#include "VeException.h"
#include "DXGI_InfoManager.h"
#include "GraphicsThrowMacros.h"

namespace ve
{
class Graphics
{
public:
	class Exception : public VeException
	{
		using VeException::VeException;
	};
	class InfoException : public Exception
	{
	public:
		InfoException( int line, const char *file, std::vector<std::string> infoMsgs ) noexcept;
		const char *what() const noexcept override;
		const char *GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class HRException : public Exception
	{
	public:
		HRException( int line, const char *file, HRESULT hr, std::vector<std::string> infoMsgs = {} ) noexcept;
		const char *what() const noexcept override;
		const char *GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class DeviceRemovedException : public HRException
	{
		using HRException::HRException;
	public:
		const char *GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics() = default;
	Graphics( const Graphics & ) = delete;
	Graphics &operator=( const Graphics & ) = delete;
	~Graphics();

	void Init( HWND hWnd );

	void EndFrame();
	void ClearBuffer( float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f ) noexcept;
private:
	bool m_initialized = false;
private:
#ifndef NDEBUG
	DXGI_InfoManager m_infoManager;
#endif
	ID3D11Device *m_pDevice = nullptr;
	IDXGISwapChain *m_pSwap = nullptr;
	ID3D11DeviceContext *m_pContext = nullptr;
	ID3D11RenderTargetView *m_pTarget = nullptr;
};
}
