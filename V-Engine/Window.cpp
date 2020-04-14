#include "Window.h"

ve::Window::WindowClass ve::Window::WindowClass::m_wndClass;

ve::Window::WindowClass::WindowClass() noexcept
	: m_hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

ve::Window::WindowClass::~WindowClass()
{
	UnregisterClass(GetName(), GetInstance());
}

ve::Window::Window(int width, int height, const char *name)
	: m_width(width),
	m_height(height)
{
	RECT wr = { 100, 100, 100 + width, 100 + height };
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	m_hWnd = CreateWindow(
		WindowClass::GetName(),
		name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
}

ve::Window::~Window()
{
	DestroyWindow(m_hWnd);
}

LRESULT ve::Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_NCCREATE )
	{
		const CREATESTRUCTW *const pCreate = reinterpret_cast<CREATESTRUCTW *>(lParam);
		Window *const wnd = static_cast<Window *>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return wnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT ve::Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window *const wnd = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return wnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT ve::Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch ( msg )
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);

}

ve::Window::Exception::Exception(int line, const char *file, HRESULT hr) noexcept
	:
	VeException(line, file),
	m_hr(hr)
{
}

const char *ve::Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << "[Type] " << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char *ve::Window::Exception::GetType() const noexcept
{
	return "VeWindow Exception";
}

std::string ve::Window::Exception::TranslateErrorCode(HRESULT hr)
{
	char *pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	if ( nMsgLen == 0 )
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT ve::Window::Exception::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string ve::Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_hr);
}
