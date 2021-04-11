#include "SaffronPCH.h"

#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>

#include "Saffron/Common/Events/KeyboardEvent.h"
#include "Saffron/Common/Events/MouseEvent.h"
#include "Saffron/Common/Run.h"
#include "Saffron/Common/ScopedLock.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Platform/Windows/WindowsWindow.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Se
{
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WindowsWindow::WindowsWindow(const Properties& props) :
	Window(props),
	_hWnd(nullptr),
	_vsync(false)
{
	static Mutex mutex;
	ScopedLock lock(mutex);
	
	_hInstance = static_cast<HINSTANCE>(GetModuleHandle(nullptr));

	const std::wstring wstringName(props.Title.begin(), props.Title.end());
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInstance;
	// TODO: Fix icon
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = wstringName.c_str();
	// TODO: Fix icon
	wc.hIconSm = nullptr;

	RegisterClassExW(&wc);

	RECT wr = {};
	wr.left = 100;
	wr.top = 100;
	wr.right = wr.left + props.Width;
	wr.bottom = wr.top + props.Height;

	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false);

	_hWnd = CreateWindowExW(0, wstringName.c_str(), wstringName.c_str(), WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
	                        CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr,
	                        _hInstance, this);

	ShowWindow(_hWnd, SW_SHOWDEFAULT);
	SE_CORE_INFO("Creating Window \"{0}\" ({1:d}x{2:d})", _title, _width, _height);

	WindowsWindow::SetVSync(true);

	PushEvent<WindowResizedEvent>(_width, _height);
}

WindowsWindow::~WindowsWindow()
{
	UnregisterClassA(_title.c_str(), _hInstance);
}

void WindowsWindow::OnUpdate()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void WindowsWindow::OnEvent(const Event& event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowResizedEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnResize));
	dispatcher.Try<WindowMovedEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnMove));
	dispatcher.Try<WindowGainedFocusEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnGainFocus));
	dispatcher.Try<WindowLostFocusEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnLostFocus));
	dispatcher.Try<WindowClosedEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnClose));
	dispatcher.Try<WindowNewTitleEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnNewTitle));
	dispatcher.Try<WindowNewIconEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnNewIcon));
	dispatcher.Try<WindowNewAntiAliasingEvent>(SE_BIND_EVENT_FN(WindowsWindow::OnNewAntiAliasing));
}

void WindowsWindow::Close()
{
}

void WindowsWindow::Focus()
{
}

void* WindowsWindow::GetNativeWindow() const
{
	return _hWnd;
}

void WindowsWindow::SetVSync(bool enabled)
{
}

bool WindowsWindow::IsVSync() const
{
	return _vsync;
}

void WindowsWindow::Minimize()
{
	Run::Later([this]
	{
		if (_maximized)
		{
			Restore();
			_minimized = true;
		}
		else if (_minimized)
		{
			Restore();
		}
		else
		{
			_minimized = true;
		}
	});
}

void WindowsWindow::Maximize()
{
	Run::Later([this]
	{
		if (_minimized)
		{
			Restore();
			_maximized = true;
		}
		else if (_maximized)
		{
			Restore();
		}
		else
		{
			_maximized = true;
		}
	});
}

bool WindowsWindow::IsMinimized() const
{
	return _minimized;
}

bool WindowsWindow::IsMaximized() const
{
	return _maximized;
}

void WindowsWindow::EnableCursor()
{
}

void WindowsWindow::DisableCursor()
{
}

void WindowsWindow::ConfineCursor()
{
	RECT rect;
	GetClientRect(_hWnd, &rect);
	MapWindowPoints(_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void WindowsWindow::FreeCursor()
{
	ClipCursor(nullptr);
}

void WindowsWindow::HideCursor()
{
	while (::ShowCursor(FALSE) >= 0);
}

void WindowsWindow::ShowCursor()
{
	while (::ShowCursor(TRUE) < 0);
}

bool WindowsWindow::OnResize(const WindowResizedEvent& event)
{
	_width = event.GetWidth();
	_height = event.GetHeight();

	/*auto *instance = this;
	Renderer::Submit([=]() { glViewport(0, 0, instance->_width, instance->_height); });
	auto &fbs = FramebufferPool::GetGlobal()->GetAll();
	for ( auto &fb : fbs )
	{
		if ( !fb->GetSpecification().NoResize )
			fb->Resize(instance->_width, instance->_height);
	}*/


	return false;
}

bool WindowsWindow::OnMove(const WindowMovedEvent& event)
{
	_position = event.GetPosition();
	return false;
}

bool WindowsWindow::OnGainFocus(const WindowGainedFocusEvent& event)
{
	return false;
}

bool WindowsWindow::OnLostFocus(const WindowLostFocusEvent& event)
{
	return false;
}

bool WindowsWindow::OnClose(const WindowClosedEvent& event)
{
	return false;
}

bool WindowsWindow::OnNewTitle(const WindowNewTitleEvent& event)
{
	_title = event.GetTitle();
	return true;
}

bool WindowsWindow::OnNewIcon(const WindowNewIconEvent& event)
{
	//GLFWimage images[1];
	//images[0].pixels = stbi_load(event.GetFilepath().string().c_str(), &images[0].width, &images[0].height, nullptr, 4); //rgba channels
	//if ( !images[0].pixels )
	//{
	//	SE_CORE_WARN("Failed to load window icon. Filepath: {0}", event.GetFilepath().string());
	//	stbi_image_free(images[0].pixels);
	//	return false;
	//}
	//glfwSetWindowIcon(m_NativeWindow, 1, images);
	//stbi_image_free(images[0].pixels);
	return true;
}

bool WindowsWindow::OnNewAntiAliasing(const WindowNewAntiAliasingEvent& event)
{
	int level = 0;
	switch (event.GetAntiAliasing())
	{
	case AntiAliasing::Off: level = 0;
		break;
	case AntiAliasing::Sample2: level = 2;
		break;
	case AntiAliasing::Sample4: level = 4;
		break;
	case AntiAliasing::Sample8: level = 8;
		break;
	case AntiAliasing::Sample16: level = 16;
		break;
	}

	/*Renderer::Submit([level]
					 {
						 if ( level )
						 {
							 glEnable(GL_MULTISAMPLE);
						 }
						 else
						 {
							 glDisable(GL_MULTISAMPLE);
						 }
						 glfwWindowHint(GLFW_SAMPLES, level);
					 });*/
	return true;
}

void WindowsWindow::Restore()
{
	_minimized = false;
	_maximized = false;
}


LRESULT CALLBACK WindowsWindow::WndSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		auto* createStructure = reinterpret_cast<CREATESTRUCTW*>(lParam);
		auto* userWindow = static_cast<WindowsWindow*>(createStructure->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(userWindow));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowsWindow::WndProc));

		return userWindow->HandleWin32Message(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WindowsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto* userWindow = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return userWindow->HandleWin32Message(hWnd, msg, wParam, lParam);
}


LRESULT WindowsWindow::HandleWin32Message(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_QUIT: PushEvent<WindowClosedEvent>();
		break;
	case WM_CLOSE: PostQuitMessage(0);
		PushEvent<WindowClosedEvent>();
		return 0;
	case WM_SETFOCUS: PushEvent<WindowGainedFocusEvent>();
		break;
	case WM_KILLFOCUS: PushEvent<WindowLostFocusEvent>();
		break;
	case WM_ACTIVATE: if (!Mouse::IsCursorEnabled())
		{
			if (wParam & WA_ACTIVE)
			{
				ConfineCursor();
				HideCursor();
			}
			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
		break;

		/*********** KEYBOARD MESSAGES ***********/
	case WM_KEYDOWN:
		// syskey commands need to be handled to track ALT key (VK_MENU) and F10
	case WM_SYSKEYDOWN:
		// stifle this keyboard message if imgui wants to capture
		/*if ( imio.WantCaptureKeyboard )
		{
			break;
		}*/
		if (lParam & 0x40000000) // filter autorepeat
		{
			PushEvent<KeyRepeatedEvent>(VKToKeyCode(static_cast<unsigned char>(wParam)));
		}
		else
		{
			PushEvent<KeyPressedEvent>(VKToKeyCode(static_cast<unsigned char>(wParam)));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// stifle this keyboard message if imgui wants to capture
		/*if ( imio.WantCaptureKeyboard )
		{
			break;
		}*/

		PushEvent<KeyReleasedEvent>(VKToKeyCode(static_cast<unsigned char>(wParam)));
		break;
	case WM_CHAR:
		// stifle this keyboard message if imgui wants to capture
		/*if ( imio.WantCaptureKeyboard )
		{
			break;
		}*/

		PushEvent<TextInputEvent>(static_cast<Uint8>(wParam));
		break;
		/*********** END KEYBOARD MESSAGES ***********/

		/************* MOUSE MESSAGES ****************/
	case WM_MOUSEMOVE:
	{
		const auto point = Win32PointToVec(lParam);
		// cursorless exclusive gets first dibs
		if (!Mouse::IsCursorEnabled())
		{
			if (!Mouse::InWindow())
			{
				SetCapture(hWnd);
				PushEvent<CursorEnteredEvent>();
				HideCursor();
			}
			break;
		}
		// stifle this mouse message if imgui wants to capture
		/*if ( imio.WantCaptureMouse )
		{
			break;
		}*/

		if (point.x >= 0 && point.x < _width && point.y >= 0 && point.y < _height)
		{
			if (!Mouse::InWindow())
			{
				SetCapture(hWnd);
				PushEvent<CursorEnteredEvent>();
			}
			PushEvent<MouseMovedEvent>(point);
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				PushEvent<MouseMovedEvent>(Vector2(point.x, point.y));
			}
			else
			{
				ReleaseCapture();
				PushEvent<CursorLeftEvent>();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SetForegroundWindow(hWnd);
		if (!Mouse::IsCursorEnabled())
		{
			ConfineCursor();
			HideCursor();
		}
		// stifle this mouse message if imgui wants to capture
		//if ( imio.WantCaptureMouse )
		//{
		//	break;
		//}
		const auto point = Win32PointToVec(lParam);
		PushEvent<MouseButtonPressedEvent>(MouseButtonCode::Left);
		PushEvent<MouseMovedEvent>(point);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// stifle this mouse message if imgui wants to capture
		//if ( imio.WantCaptureMouse )
		//{
		//	break;
		//}
		const auto point = Win32PointToVec(lParam);
		PushEvent<MouseButtonPressedEvent>(MouseButtonCode::Right);
		PushEvent<MouseMovedEvent>(point);
		break;
	}
	case WM_LBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
		//if ( imio.WantCaptureMouse )
		//{
		//	break;
		//}

		const auto point = Win32PointToVec(lParam);
		PushEvent<MouseButtonReleasedEvent>(MouseButtonCode::Left);
		PushEvent<MouseMovedEvent>(point);
		// release mouse if outside of window
		if (point.x < 0 || point.x >= _width || point.y < 0 || point.y >= _height)
		{
			ReleaseCapture();
			PushEvent<CursorLeftEvent>();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
		//if ( imio.WantCaptureMouse )
		//{
		//	break;
		//}
		const auto point = Win32PointToVec(lParam);
		PushEvent<MouseButtonReleasedEvent>(MouseButtonCode::Right);
		PushEvent<MouseMovedEvent>(point);
		// release mouse if outside of window
		if (point.x < 0 || point.x >= _width || point.y < 0 || point.y >= _height)
		{
			ReleaseCapture();
			PushEvent<CursorLeftEvent>();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// stifle this mouse message if imgui wants to capture
		//if ( imio.WantCaptureMouse )
		//{
		//	break;
		//}
		const auto point = Win32PointToVec(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		PushEvent<MouseWheelScrolledEvent>(MouseWheelCode::VerticalWheel, delta);
		PushEvent<MouseMovedEvent>(point);
		break;
	}
		/************** END MOUSE MESSAGES **************/

		/************** RAW MOUSE MESSAGES **************/
	case WM_INPUT:
	{
		if (!Mouse::IsRawInputEnabled())
		{
			break;
		}
		UINT size = 0;
		// first get the size of the input data
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size,
		                    sizeof(RAWINPUTHEADER)) == -1)
		{
			// bail msg processing if error
			break;
		}
		_rawInputBuffer.resize(size);
		// read in the input data
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, _rawInputBuffer.data(), &size,
		                    sizeof(RAWINPUTHEADER)) != size)
		{
			// bail msg processing if error
			break;
		}
		// process the raw input data
		auto& ri = reinterpret_cast<const RAWINPUT&>(*_rawInputBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE && (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
		{
			PushEvent<MouseMovedRawEvent>(Vector2(static_cast<float>(ri.data.mouse.lLastX),
			                                      static_cast<float>(ri.data.mouse.lLastY)));
		}
		break;
	}
		/************** END RAW MOUSE MESSAGES **************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

KeyCode WindowsWindow::VKToKeyCode(unsigned char vkkey)
{
	if (static_cast<int>(vkkey) > 0x30 && static_cast<int>(vkkey) < 0x5A)
	{
		switch (vkkey)
		{
		case 'A': return KeyCode::A;
		case 'B': return KeyCode::B;
		case 'C': return KeyCode::C;
		case 'D': return KeyCode::D;
		case 'E': return KeyCode::E;
		case 'F': return KeyCode::F;
		case 'G': return KeyCode::G;
		case 'H': return KeyCode::H;
		case 'I': return KeyCode::I;
		case 'J': return KeyCode::J;
		case 'K': return KeyCode::K;
		case 'L': return KeyCode::L;
		case 'M': return KeyCode::M;
		case 'N': return KeyCode::N;
		case 'O': return KeyCode::O;
		case 'P': return KeyCode::P;
		case 'Q': return KeyCode::Q;
		case 'R': return KeyCode::R;
		case 'S': return KeyCode::S;
		case 'T': return KeyCode::T;
		case 'U': return KeyCode::U;
		case 'V': return KeyCode::V;
		case 'W': return KeyCode::W;
		case 'X': return KeyCode::X;
		case 'Y': return KeyCode::Y;
		case 'Z': return KeyCode::Z;
		case '0': return KeyCode::Num0;
		case '1': return KeyCode::Num1;
		case '2': return KeyCode::Num2;
		case '3': return KeyCode::Num3;
		case '4': return KeyCode::Num4;
		case '5': return KeyCode::Num5;
		case '6': return KeyCode::Num6;
		case '7': return KeyCode::Num7;
		case '8': return KeyCode::Num8;
		case '9': return KeyCode::Num9;
		}
	}

	switch (vkkey)
	{
	case VK_ESCAPE: return KeyCode::Escape;
	case VK_LCONTROL: return KeyCode::LControl;
	case VK_SHIFT: return KeyCode::LShift;
	case VK_LMENU: return KeyCode::LAlt;
	case VK_LWIN: return KeyCode::LSystem;
	case VK_RCONTROL: return KeyCode::RControl;
	case VK_RMENU: return KeyCode::RAlt;
	case VK_RWIN: return KeyCode::RSystem;
	case VK_APPS: return KeyCode::Menu;
	case VK_OEM_4: return KeyCode::LBracket;
	case VK_OEM_6: return KeyCode::RBracket;
	case VK_OEM_1: return KeyCode::Semicolon;
	case VK_OEM_COMMA: return KeyCode::Comma;
	case VK_OEM_PERIOD: return KeyCode::Period;
	case VK_OEM_7: return KeyCode::Quote;
	case VK_OEM_2: return KeyCode::Slash;
	case VK_OEM_5: return KeyCode::Backslash;
	case VK_OEM_3: return KeyCode::Tilde;
	case VK_OEM_PLUS: return KeyCode::Equal;
	case VK_OEM_MINUS: return KeyCode::Hyphen;
	case VK_SPACE: return KeyCode::Space;
	case VK_RETURN: return KeyCode::Enter;
	case VK_BACK: return KeyCode::Backspace;
	case VK_TAB: return KeyCode::Tab;
	case VK_PRIOR: return KeyCode::PageUp;
	case VK_NEXT: return KeyCode::PageDown;
	case VK_END: return KeyCode::End;
	case VK_HOME: return KeyCode::Home;
	case VK_INSERT: return KeyCode::Insert;
	case VK_DELETE: return KeyCode::Delete;
	case VK_ADD: return KeyCode::Add;
	case VK_SUBTRACT: return KeyCode::Subtract;
	case VK_MULTIPLY: return KeyCode::Multiply;
	case VK_DIVIDE: return KeyCode::Divide;
	case VK_LEFT: return KeyCode::Left;
	case VK_RIGHT: return KeyCode::Right;
	case VK_UP: return KeyCode::Up;
	case VK_DOWN: return KeyCode::Down;
	case VK_NUMPAD0: return KeyCode::Numpad0;
	case VK_NUMPAD1: return KeyCode::Numpad1;
	case VK_NUMPAD2: return KeyCode::Numpad2;
	case VK_NUMPAD3: return KeyCode::Numpad3;
	case VK_NUMPAD4: return KeyCode::Numpad4;
	case VK_NUMPAD5: return KeyCode::Numpad5;
	case VK_NUMPAD6: return KeyCode::Numpad6;
	case VK_NUMPAD7: return KeyCode::Numpad7;
	case VK_NUMPAD8: return KeyCode::Numpad8;
	case VK_NUMPAD9: return KeyCode::Numpad9;
	case VK_F1: return KeyCode::F1;
	case VK_F2: return KeyCode::F2;
	case VK_F3: return KeyCode::F3;
	case VK_F4: return KeyCode::F4;
	case VK_F5: return KeyCode::F5;
	case VK_F6: return KeyCode::F6;
	case VK_F7: return KeyCode::F7;
	case VK_F8: return KeyCode::F8;
	case VK_F9: return KeyCode::F9;
	case VK_F10: return KeyCode::F10;
	case VK_F11: return KeyCode::F11;
	case VK_F12: return KeyCode::F12;
	case VK_F13: return KeyCode::F13;
	case VK_F14: return KeyCode::F14;
	case VK_F15: return KeyCode::F15;
	case VK_PAUSE: return KeyCode::Pause;
	default: return KeyCode::Unknown;
	}
}

Vector2 WindowsWindow::Win32PointToVec(LPARAM lParam)
{
	const auto win32Points = MAKEPOINTS(lParam);
	return Vector2(static_cast<float>(win32Points.x), static_cast<float>(win32Points.y));
}
}
