#include "Graphics.h"

#include "dxerr.h"
#include "GraphicsThrowMacros.h"

void ve::Graphics::Init( HWND hWnd )
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

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = {};

	GFX_THROW_INFO(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swapCreateFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&sd,
			&m_pSwap,
			&m_pDevice,
			nullptr,
			&m_pContext
		)
	);

	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(
		m_pSwap->GetBuffer(
			0,
			__uuidof( ID3D11Resource ),
			&pBackBuffer
		)
	);
	GFX_THROW_INFO(
		m_pDevice->CreateRenderTargetView(
			pBackBuffer.Get(),
			nullptr,
			&m_pTarget
		)
	);

	m_initialized = true;
}

void ve::Graphics::EndFrame()
{
	HRESULT hr = {};
#ifndef NDEBUG
	m_infoManager.Set();
#endif

	if ( FAILED( m_pSwap->Present( 1u, 0u ) ) )
	{
		if ( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw GFX_DEVICE_REMOVED_EXCEPT( m_pDevice->GetDeviceRemovedReason() );
		}
		else
		{
			throw GFX_EXCEPT( hr );
		}
	}
}

void ve::Graphics::ClearBuffer( float red, float green, float blue, float alpha ) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	m_pContext->ClearRenderTargetView( m_pTarget.Get(), color );
}

ve::Graphics::HRException::HRException( int line, const char *file, HRESULT hr, std::vector<std::string> infoMsgs ) noexcept
	:
	Exception( line, file ),
	hr( hr )
{
	for ( const auto &msg : infoMsgs )
	{
		info += msg;
		info.push_back( '\n' );
	}

	//Remove final newline, if exists
	if ( !info.empty() )
	{
		info.pop_back();
	}
}

const char *ve::Graphics::HRException::what() const noexcept
{
	std::ostringstream oss;
	oss << "[Type] " << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if ( !info.empty() )
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char *ve::Graphics::HRException::GetType() const noexcept
{
	return "V-Engine Graphics Exception";
}

HRESULT ve::Graphics::HRException::GetErrorCode() const noexcept
{
	return hr;
}

std::string ve::Graphics::HRException::GetErrorString() const noexcept
{
	return DXGetErrorString( hr );
}

std::string ve::Graphics::HRException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription( hr, buf, sizeof( buf ) );
	return buf;
}

std::string ve::Graphics::HRException::GetErrorInfo() const noexcept
{
	return info;
}

const char *ve::Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "V-Engine Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

ve::Graphics::InfoException::InfoException( int line, const char *file, std::vector<std::string> infoMsgs ) noexcept
	:
	Exception( line, file )
{
	for ( const auto &msg : infoMsgs )
	{
		info += msg;
		info.push_back( '\n' );
	}

	//Remove final newline, if exists
	if ( !info.empty() )
	{
		info.pop_back();
	}
}

const char *ve::Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << "[Type] " << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char *ve::Graphics::InfoException::GetType() const noexcept
{
	return "V-Engine Graphics Info Exception";
}

std::string ve::Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
