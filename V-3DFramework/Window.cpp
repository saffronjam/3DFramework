#include "Window.h"
#include "resource.h"

Window::Win32Window Window::Win32Window::m_win32Window;

Window::Win32Window::Win32Window() noexcept
	: m_instanceHandle(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(m_instanceHandle, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(m_instanceHandle, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	RegisterClassEx(&wc);
}

Window::Win32Window::~Win32Window()
{
	UnregisterClass(GetName(), GetInstance());
}

Window::Window(int width, int height, const char* name)
	: m_width(width),
	m_height(height)
{
	RECT wr = { 100, 100, 100 + width, 100 + height };

	if ( AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0 )
	{
		throw VEWND_LAST_EXCEPT();
	}
	m_windowHandle = CreateWindow(
		Win32Window::GetName(),
		name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, Win32Window::GetInstance(), this
	);
	if ( m_windowHandle == nullptr )
	{
		throw VEWND_LAST_EXCEPT();
	}

	ShowWindow(m_windowHandle, SW_SHOWDEFAULT);

	gfx.Init(m_windowHandle);
}

Window::~Window()
{
	DestroyWindow(m_windowHandle);
}

void Window::SetTitle(const std::string& title)
{
	if ( SetWindowText(m_windowHandle, title.c_str()) == 0 )
	{
		throw VEWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
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

LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ( msg == WM_NCCREATE )
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const wnd = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return wnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return wnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if ( ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) )
	{
		return true;
	}
	const auto& imio = ImGui::GetIO();

	switch ( msg )
	{
		// we don't want the DefProc to handle this message because
		// we want our destructor to destroy the window, so return 0 instead of break
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// clear keystate when window loses focus to prevent input getting "stuck"
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;

		/*********** KEYBOARD MESSAGES ***********/
	case WM_KEYDOWN:
		// syskey commands need to be handled to track ALT key (VK_MENU) and F10
	case WM_SYSKEYDOWN:
		// stifle this keyboard message if imgui wants to capture
		if ( imio.WantCaptureKeyboard )
		{
			break;
		}
		if ( !(lParam & 0x40000000) || kbd.AutoRepeatIsOn() ) // filter autorepeat
		{
			kbd.OnKeyPress(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// stifle this keyboard message if imgui wants to capture
		if ( imio.WantCaptureKeyboard )
		{
			break;
		}
		kbd.OnKeyRelease(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		// stifle this keyboard message if imgui wants to capture
		if ( imio.WantCaptureKeyboard )
		{
			break;
		}
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/*********** END KEYBOARD MESSAGES ***********/

		/************* MOUSE MESSAGES ****************/
	case WM_MOUSEMOVE:
	{
		// stifle this mouse message if imgui wants to capture
		if ( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		// in client region -> log move, and log enter + capture mouse (if not previously in window)
		if ( pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height )
		{
			mouse.OnMove(pt.x, pt.y);
			if ( !mouse.IsInWindow() )
			{
				SetCapture(hWnd);
				mouse.OnEnter(pt.x, pt.y);
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if ( wParam & (MK_LBUTTON | MK_RBUTTON) )
			{
				mouse.OnMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
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
		SetForegroundWindow(hWnd);
		// stifle this mouse message if imgui wants to capture
		if ( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPress(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// stifle this mouse message if imgui wants to capture
		if ( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPress(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
		if ( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftRelease(pt.x, pt.y);
		// release mouse if outside of window
		if ( pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height )
		{
			ReleaseCapture();
			mouse.OnLeave(pt.x, pt.y);
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
		if ( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightRelease(pt.x, pt.y);
		// release mouse if outside of window
		if ( pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height )
		{
			ReleaseCapture();
			mouse.OnLeave(pt.x, pt.y);
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// stifle this mouse message if imgui wants to capture
		if ( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	/************** END MOUSE MESSAGES **************/
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	VeException(line, file),
	m_hr(hr)
{
}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << "[Type] " << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "V-Engine Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr)
{
	char* pMsgBuf = nullptr;
	const DWORD nMsgLen = FormatMessage(
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

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_hr);
}
