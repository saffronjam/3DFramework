#include "Window.h"
#include "resource.h"

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
	wc.hIcon = static_cast<HICON>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
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

	if ( AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0 )
	{
		throw VEWND_LAST_EXCEPT();
	}
	m_hWnd = CreateWindow(
		WindowClass::GetName(),
		name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	if ( m_hWnd == nullptr )
	{
		throw VEWND_LAST_EXCEPT();
	}

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	gfx.Init(m_hWnd);
}

ve::Window::~Window()
{
	DestroyWindow(m_hWnd);
}

void ve::Window::SetTitle(const std::string &title)
{
	if ( SetWindowText(m_hWnd, title.c_str()) == 0 )
	{
		throw VEWND_LAST_EXCEPT();
	}
}

std::optional<int> ve::Window::ProcessMessages()
{
	MSG msg;
	while ( PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) )
	{
		if ( msg.message == WM_QUIT )
		{
			return static_cast<int>(msg.wParam);
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return std::nullopt;
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
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;

	/********************* KEYBOARD ***************************/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if ( !(lParam & 0x40000000) || kbd.AutoRepeatIsOn() )
		{
			kbd.OnKeyPress(static_cast<uint8_t>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyRelease(static_cast<uint8_t>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<char>(wParam));
		break;
	/********************** MOUSE *****************************/
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		if ( pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height )
		{
			mouse.OnMove(pt.x, pt.y);
			if ( !mouse.IsInWindow() )
			{
				SetCapture(hWnd);
				mouse.OnEnter(pt.x, pt.y);
			}
		}
		else
		{
			if ( mouse.LeftIsDown() || mouse.RightIsDown() )
			{
				mouse.OnMove(pt.x, pt.y);
			}
			else
			{
				ReleaseCapture();
				mouse.OnLeave(pt.x, pt.y);
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPress(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPress(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftRelease(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightRelease(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
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
