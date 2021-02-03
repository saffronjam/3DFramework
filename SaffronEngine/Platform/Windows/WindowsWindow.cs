using System;
using System.Drawing;
using System.Runtime.InteropServices;
using SaffronEngine.Common;
using Serilog;

namespace SaffronEngine.Platform.Windows
{
    public class WindowsWindow : Window
    {
        private readonly WNDPROC _wndproc;
        private readonly IntPtr _hwnd;

        public override string Title
        {
            get => base.Title;
            set
            {
                if (Title != value)
                {
                    var lpString = Marshal.StringToHGlobalAnsi(value);
                    User32.SetWindowTextA(_hwnd, lpString);
                }

                base.Title = value;
            }
        }

        public override IntPtr Handle => _hwnd;

        public WindowsWindow(string title, uint width, uint height) : base(title, width, height)
        {
            var instance = Kernel32.GetModuleHandleW(null);
            var icon = User32.LoadIconW(IntPtr.Zero, IDI.APPLICATION);
            _wndproc = WndProc;

            // register the window class
            var wndclass = new WNDCLASSEXW
            {
                cbSize = (uint) Marshal.SizeOf<WNDCLASSEXW>(),
                style = CS.HREDRAW | CS.VREDRAW | CS.OWNDC,
                hInstance = instance,
                hIcon = icon,
                hIconSm = icon,
                hCursor = User32.LoadCursorW(IntPtr.Zero, IDC.ARROW),
                lpszClassName = ClassName,
                lpfnWndProc = Marshal.GetFunctionPointerForDelegate(_wndproc)
            };

            var atom = User32.RegisterClassExW(ref wndclass);
            if (atom == 0)
                throw new InvalidOperationException("Failed to register window class");

            // size the client area appropriately
            const WS styles = WS.OVERLAPPEDWINDOW;
            var rect = new RECT {left = 0, top = 0, right = (int) width, bottom = (int) height};
            if (User32.AdjustWindowRectEx(ref rect, styles, 0, 0))
            {
                width = (uint) (rect.right - rect.left);
                height = (uint) (rect.bottom - rect.top);
            }

            // create the window
            _hwnd = User32.CreateWindowExW(
                0,
                new IntPtr(atom),
                title,
                styles,
                CW.USEDEFAULT,
                CW.USEDEFAULT,
                (int) width,
                (int) height,
                IntPtr.Zero,
                IntPtr.Zero,
                instance,
                IntPtr.Zero
            );
            if (_hwnd == IntPtr.Zero)
                throw new InvalidOperationException("Failed to create window");
        }

        public override void Show()
        {
            User32.ShowWindow(_hwnd, SW.SHOWDEFAULT);
            User32.UpdateWindow(_hwnd);
        }

        public override void Close()
        {
            User32.PostMessage(_hwnd, WM.GAME_DONE, IntPtr.Zero, IntPtr.Zero);
        }

        public override int RunMessageLoop()
        {
            MSG msg;
            while (User32.GetMessage(out msg, IntPtr.Zero, 0, 0) != 0)
            {
                User32.TranslateMessage(ref msg);
                User32.DispatchMessage(ref msg);
            }

            return TOINT32(msg.wParam);
        }

        private IntPtr WndProc(IntPtr hwnd, WM msg, IntPtr wparam, IntPtr lparam)
        {
            switch (msg)
            {
                case WM.ERASEBKGND:
                    return TRUE;

                case WM.PAINT:
                    User32.ValidateRect(hwnd, IntPtr.Zero);
                    return IntPtr.Zero;

                case WM.SIZE:
                {
                    var width = (uint) LOWORD(lparam);
                    var height = (uint) HIWORD(lparam);
                    PushEvent(new Event {Type = EventType.Resized, Size = {Width = width, Height = height}});
                    return IntPtr.Zero;
                }

                case WM.CLOSE:
                    PushEvent(new Event {Type = EventType.Closed});
                    return IntPtr.Zero;

                case WM.GAME_DONE:
                    User32.DestroyWindow(hwnd);
                    return IntPtr.Zero;

                case WM.DESTROY:
                    User32.PostQuitMessage(0);
                    return IntPtr.Zero;

                /*********** KEYBOARD MESSAGES ***********/
                case WM.KEYDOWN:
                // syskey commands need to be handled to track ALT key (VK_MENU) and F10
                case WM.SYSKEYDOWN:
                {
                    var code = VKToKeyCode((VK) wparam);
                    PushEvent(new Event {Type = EventType.KeyPressed, Key = new KeyEvent {Code = code}});
                    break;
                }
                case WM.KEYUP:
                case WM.SYSKEYUP:
                {
                    var code = VKToKeyCode((VK) wparam);
                    PushEvent(new Event {Type = EventType.KeyReleased, Key = new KeyEvent {Code = code}});
                    break;
                }
                case WM.CHAR:
                    var unicode = (uint) VKToKeyCode((VK) wparam);
                    PushEvent(new Event {Type = EventType.TextEntered, Text = new TextEvent {Unicode = unicode}});
                    break;
                /*********** END KEYBOARD MESSAGES ***********/

                /************* MOUSE MESSAGES ****************/
                case WM.MOUSEMOVE:
                {
                    var point = GetPoint(lparam);
                    var winSize = Application.Instance.Window.Size;
                    var mouseMoveEvent = new MouseMoveEvent {X = point.X, Y = point.Y};

                    // in client region -> log move, and log enter + capture mouse (if not previously in window)
                    if (point.X >= 0 && point.X < winSize.X && point.Y >= 0 && point.Y < winSize.Y)
                    {
                        PushEvent(new Event {Type = EventType.MouseMoved, MouseMove = mouseMoveEvent});
                        if (!Input.IsMouseInWindow())
                        {
                            User32.SetCapture(hwnd);
                            PushEvent(new Event {Type = EventType.MouseEntered});
                        }
                    }
                    // not in client -> log move / maintain capture if button down
                    else
                    {
                        if (((int) wparam & ((int) MK.LBUTTON | (int) MK.RBUTTON)) != 0x0)
                        {
                            PushEvent(new Event {Type = EventType.MouseMoved, MouseMove = mouseMoveEvent});
                        }
                        // button up -> release capture / log event for leaving
                        else
                        {
                            User32.ReleaseCapture();
                            PushEvent(new Event {Type = EventType.MouseLeft});
                        }
                    }


                    break;
                }
                case WM.LBUTTONDOWN:
                {
                    var mouseButtonEvent = new MouseButtonEvent {Button = MouseButtonCode.Left};
                    PushEvent(new Event {Type = EventType.MouseButtonPressed, MouseButton = mouseButtonEvent});
                    // SetForegroundWindow(hWnd);
                    // if (!cursorEnabled)
                    // {
                    //     ConfineCursor();
                    //     HideCursor();
                    // }

                    // const POINTS pt = MAKEPOINTS(lparam);
                    // mouse.OnLeftPressed(pt.x, pt.y);
                    break;
                }
                case WM.RBUTTONDOWN:
                {
                    var mouseButtonEvent = new MouseButtonEvent {Button = MouseButtonCode.Right};
                    PushEvent(new Event {Type = EventType.MouseButtonPressed, MouseButton = mouseButtonEvent});
                    // const POINTS pt = MAKEPOINTS(lparam);
                    // mouse.OnRightPressed(pt.x, pt.y);
                    break;
                }
                case WM.LBUTTONUP:
                {
                    var mouseButtonEvent = new MouseButtonEvent {Button = MouseButtonCode.Left};
                    PushEvent(new Event {Type = EventType.MouseButtonReleased, MouseButton = mouseButtonEvent});
                    // const POINTS pt = MAKEPOINTS(lparam);
                    // mouse.OnLeftReleased(pt.x, pt.y);
                    // // release mouse if outside of window
                    // if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
                    // {
                    //     ReleaseCapture();
                    //     mouse.OnMouseLeave();
                    // }

                    break;
                }
                case WM.RBUTTONUP:
                {
                    var mouseButtonEvent = new MouseButtonEvent {Button = MouseButtonCode.Right};
                    PushEvent(new Event {Type = EventType.MouseButtonReleased, MouseButton = mouseButtonEvent});
                    // const POINTS pt = MAKEPOINTS(lparam);
                    // mouse.OnRightReleased(pt.x, pt.y);
                    // // release mouse if outside of window
                    // if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
                    // {
                    //     ReleaseCapture();
                    //     mouse.OnMouseLeave();
                    // }

                    break;
                }
                case WM.MOUSEWHEEL:
                {
                    var delta = GET_WHEEL_DELTA_WPARAM(wparam);
                    var point = GetPoint(wparam);
                    var mouseWheelEvent = new MouseWheelScrollEvent
                        {Delta = delta, X = point.X, Y = point.Y, Wheel = MouseWheelCode.HorizontalWheel};
                    PushEvent(new Event {Type = EventType.MouseButtonReleased, MouseWheelScroll = mouseWheelEvent});
                    // const POINTS pt = MAKEPOINTS(lparam);
                    // const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                    // mouse.OnWheelDelta(pt.x, pt.y, delta);
                    break;
                }
                /************** END MOUSE MESSAGES **************/

                /************** RAW MOUSE MESSAGES **************/
                case WM.INPUT:
                {
                    // if (!mouse.RawEnabled())
                    // {
                    //     break;
                    // }
                    //
                    // UINT size;
                    // // first get the size of the input data
                    // if (GetRawInputData(
                    //     reinterpret_cast<HRAWINPUT>(lparam),
                    //     RID_INPUT,
                    //     nullptr,
                    //     &size,
                    //     sizeof(RAWINPUTHEADER)) == -1)
                    // {
                    //     // bail msg processing if error
                    //     break;
                    // }
                    //
                    // rawBuffer.resize(size);
                    // // read in the input data
                    // if (GetRawInputData(
                    //     reinterpret_cast<HRAWINPUT>(lparam),
                    //     RID_INPUT,
                    //     rawBuffer.data(),
                    //     &size,
                    //     sizeof(RAWINPUTHEADER)) != size)
                    // {
                    //     // bail msg processing if error
                    //     break;
                    // }
                    //
                    // // process the raw input data
                    // auto & ri = reinterpret_cast <const RAWINPUT  &>(*rawBuffer.data());
                    // if (ri.header.dwType == RIM_TYPEMOUSE &&
                    //     (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
                    // {
                    //     mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
                    // }

                    break;
                }
                /************** END RAW MOUSE MESSAGES **************/
                case WM.NULL:
                    break;
                case WM.CREATE:
                    break;
                case WM.MOVE:
                    break;
                case WM.ACTIVATE:
                {
                    PushEvent(LOWORD(wparam) == (int) WA.INACTIVE
                        ? new Event {Type = EventType.LostFocus}
                        : new Event {Type = EventType.GainedFocus});
                    break;
                }
                case WM.SETFOCUS:
                    break;
                case WM.KILLFOCUS:
                    break;
                case WM.ENABLE:
                    break;
                case WM.SETREDRAW:
                    break;
                case WM.SETTEXT:
                    break;
                case WM.GETTEXT:
                    break;
                case WM.GETTEXTLENGTH:
                    break;
                case WM.QUERYENDSESSION:
                    break;
                case WM.QUERYOPEN:
                    break;
                case WM.ENDSESSION:
                    break;
                case WM.QUIT:
                    break;
                case WM.SYSCOLORCHANGE:
                    break;
                case WM.SHOWWINDOW:
                    break;
                case WM.WININICHANGE:
                    break;
                case WM.DEVMODECHANGE:
                    break;
                case WM.ACTIVATEAPP:
                    break;
                case WM.FONTCHANGE:
                    break;
                case WM.TIMECHANGE:
                    break;
                case WM.CANCELMODE:
                    break;
                case WM.SETCURSOR:
                    break;
                case WM.MOUSEACTIVATE:
                    break;
                case WM.CHILDACTIVATE:
                    break;
                case WM.QUEUESYNC:
                    break;
                case WM.GETMINMAXINFO:
                    break;
                case WM.PAINTICON:
                    break;
                case WM.ICONERASEBKGND:
                    break;
                case WM.NEXTDLGCTL:
                    break;
                case WM.SPOOLERSTATUS:
                    break;
                case WM.DRAWITEM:
                    break;
                case WM.MEASUREITEM:
                    break;
                case WM.DELETEITEM:
                    break;
                case WM.VKEYTOITEM:
                    break;
                case WM.CHARTOITEM:
                    break;
                case WM.SETFONT:
                    break;
                case WM.GETFONT:
                    break;
                case WM.SETHOTKEY:
                    break;
                case WM.GETHOTKEY:
                    break;
                case WM.QUERYDRAGICON:
                    break;
                case WM.COMPAREITEM:
                    break;
                case WM.GETOBJECT:
                    break;
                case WM.COMPACTING:
                    break;
                case WM.COMMNOTIFY:
                    break;
                case WM.WINDOWPOSCHANGING:
                    break;
                case WM.WINDOWPOSCHANGED:
                    break;
                case WM.POWER:
                    break;
                case WM.COPYDATA:
                    break;
                case WM.CANCELJOURNAL:
                    break;
                case WM.NOTIFY:
                    break;
                case WM.INPUTLANGCHANGEREQUEST:
                    break;
                case WM.INPUTLANGCHANGE:
                    break;
                case WM.TCARD:
                    break;
                case WM.HELP:
                    break;
                case WM.USERCHANGED:
                    break;
                case WM.NOTIFYFORMAT:
                    break;
                case WM.CONTEXTMENU:
                    break;
                case WM.STYLECHANGING:
                    break;
                case WM.STYLECHANGED:
                    break;
                case WM.DISPLAYCHANGE:
                    break;
                case WM.GETICON:
                    break;
                case WM.SETICON:
                    break;
                case WM.NCCREATE:
                    break;
                case WM.NCDESTROY:
                    break;
                case WM.NCCALCSIZE:
                    break;
                case WM.NCHITTEST:
                    break;
                case WM.NCPAINT:
                    break;
                case WM.NCACTIVATE:
                    break;
                case WM.GETDLGCODE:
                    break;
                case WM.SYNCPAINT:
                    break;
                case WM.NCMOUSEMOVE:
                    break;
                case WM.NCLBUTTONDOWN:
                    break;
                case WM.NCLBUTTONUP:
                    break;
                case WM.NCLBUTTONDBLCLK:
                    break;
                case WM.NCRBUTTONDOWN:
                    break;
                case WM.NCRBUTTONUP:
                    break;
                case WM.NCRBUTTONDBLCLK:
                    break;
                case WM.NCMBUTTONDOWN:
                    break;
                case WM.NCMBUTTONUP:
                    break;
                case WM.NCMBUTTONDBLCLK:
                    break;
                case WM.NCXBUTTONDOWN:
                    break;
                case WM.NCXBUTTONUP:
                    break;
                case WM.NCXBUTTONDBLCLK:
                    break;
                case WM.INPUT_DEVICE_CHANGE:
                    break;
                case WM.DEADCHAR:
                    break;
                case WM.SYSCHAR:
                    break;
                case WM.SYSDEADCHAR:
                    break;
                case WM.UNICHAR:
                    break;
                case WM.IME_STARTCOMPOSITION:
                    break;
                case WM.IME_ENDCOMPOSITION:
                    break;
                case WM.IME_COMPOSITION:
                    break;
                case WM.INITDIALOG:
                    break;
                case WM.COMMAND:
                    break;
                case WM.SYSCOMMAND:
                    break;
                case WM.TIMER:
                    break;
                case WM.HSCROLL:
                    break;
                case WM.VSCROLL:
                    break;
                case WM.INITMENU:
                    break;
                case WM.INITMENUPOPUP:
                    break;
                case WM.MENUSELECT:
                    break;
                case WM.MENUCHAR:
                    break;
                case WM.ENTERIDLE:
                    break;
                case WM.MENURBUTTONUP:
                    break;
                case WM.MENUDRAG:
                    break;
                case WM.MENUGETOBJECT:
                    break;
                case WM.UNINITMENUPOPUP:
                    break;
                case WM.MENUCOMMAND:
                    break;
                case WM.CHANGEUISTATE:
                    break;
                case WM.UPDATEUISTATE:
                    break;
                case WM.QUERYUISTATE:
                    break;
                case WM.CTLCOLORMSGBOX:
                    break;
                case WM.CTLCOLOREDIT:
                    break;
                case WM.CTLCOLORLISTBOX:
                    break;
                case WM.CTLCOLORBTN:
                    break;
                case WM.CTLCOLORDLG:
                    break;
                case WM.CTLCOLORSCROLLBAR:
                    break;
                case WM.CTLCOLORSTATIC:
                    break;
                case WM.MN_GETHMENU:
                    break;
                case WM.LBUTTONDBLCLK:
                    break;
                case WM.RBUTTONDBLCLK:
                    break;
                case WM.MBUTTONDOWN:
                    break;
                case WM.MBUTTONUP:
                    break;
                case WM.MBUTTONDBLCLK:
                    break;
                case WM.XBUTTONDOWN:
                    break;
                case WM.XBUTTONUP:
                    break;
                case WM.XBUTTONDBLCLK:
                    break;
                case WM.MOUSEHWHEEL:
                    break;
                case WM.PARENTNOTIFY:
                    break;
                case WM.ENTERMENULOOP:
                    break;
                case WM.EXITMENULOOP:
                    break;
                case WM.NEXTMENU:
                    break;
                case WM.SIZING:
                    break;
                case WM.CAPTURECHANGED:
                    break;
                case WM.MOVING:
                    break;
                case WM.POWERBROADCAST:
                    break;
                case WM.DEVICECHANGE:
                    break;
                case WM.MDICREATE:
                    break;
                case WM.MDIDESTROY:
                    break;
                case WM.MDIACTIVATE:
                    break;
                case WM.MDIRESTORE:
                    break;
                case WM.MDINEXT:
                    break;
                case WM.MDIMAXIMIZE:
                    break;
                case WM.MDITILE:
                    break;
                case WM.MDICASCADE:
                    break;
                case WM.MDIICONARRANGE:
                    break;
                case WM.MDIGETACTIVE:
                    break;
                case WM.MDISETMENU:
                    break;
                case WM.ENTERSIZEMOVE:
                    break;
                case WM.EXITSIZEMOVE:
                    break;
                case WM.DROPFILES:
                    break;
                case WM.MDIREFRESHMENU:
                    break;
                case WM.IME_SETCONTEXT:
                    break;
                case WM.IME_NOTIFY:
                    break;
                case WM.IME_CONTROL:
                    break;
                case WM.IME_COMPOSITIONFULL:
                    break;
                case WM.IME_SELECT:
                    break;
                case WM.IME_CHAR:
                    break;
                case WM.IME_REQUEST:
                    break;
                case WM.IME_KEYDOWN:
                    break;
                case WM.IME_KEYUP:
                    break;
                case WM.MOUSEHOVER:
                    break;
                case WM.MOUSELEAVE:
                    break;
                case WM.NCMOUSEHOVER:
                    break;
                case WM.NCMOUSELEAVE:
                    break;
                case WM.WTSSESSION_CHANGE:
                    break;
                case WM.TABLET_FIRST:
                    break;
                case WM.TABLET_LAST:
                    break;
                case WM.CUT:
                    break;
                case WM.COPY:
                    break;
                case WM.PASTE:
                    break;
                case WM.CLEAR:
                    break;
                case WM.UNDO:
                    break;
                case WM.RENDERFORMAT:
                    break;
                case WM.RENDERALLFORMATS:
                    break;
                case WM.DESTROYCLIPBOARD:
                    break;
                case WM.DRAWCLIPBOARD:
                    break;
                case WM.PAINTCLIPBOARD:
                    break;
                case WM.VSCROLLCLIPBOARD:
                    break;
                case WM.SIZECLIPBOARD:
                    break;
                case WM.ASKCBFORMATNAME:
                    break;
                case WM.CHANGECBCHAIN:
                    break;
                case WM.HSCROLLCLIPBOARD:
                    break;
                case WM.QUERYNEWPALETTE:
                    break;
                case WM.PALETTEISCHANGING:
                    break;
                case WM.PALETTECHANGED:
                    break;
                case WM.HOTKEY:
                    break;
                case WM.PRINT:
                    break;
                case WM.PRINTCLIENT:
                    break;
                case WM.APPCOMMAND:
                    break;
                case WM.THEMECHANGED:
                    break;
                case WM.CLIPBOARDUPDATE:
                    break;
                case WM.DWMCOMPOSITIONCHANGED:
                    break;
                case WM.DWMNCRENDERINGCHANGED:
                    break;
                case WM.DWMCOLORIZATIONCOLORCHANGED:
                    break;
                case WM.DWMWINDOWMAXIMIZEDCHANGE:
                    break;
                case WM.GETTITLEBARINFOEX:
                    break;
                case WM.HANDHELDFIRST:
                    break;
                case WM.HANDHELDLAST:
                    break;
                case WM.AFXFIRST:
                    break;
                case WM.AFXLAST:
                    break;
                case WM.PENWINFIRST:
                    break;
                case WM.PENWINLAST:
                    break;
                case WM.APP:
                    break;
                case WM.REFLECT:
                    break;
            }

            return User32.DefWindowProcW(hwnd, msg, wparam, lparam);
        }

        private static Point GetPoint(IntPtr _xy)
        {
            var xy = unchecked(IntPtr.Size == 8 ? (uint) _xy.ToInt64() : (uint) _xy.ToInt32());
            int x = unchecked((short) xy);
            int y = unchecked((short) (xy >> 16));
            return new Point(x, y);
        }

        private static ushort HIWORD(IntPtr wparam) => (ushort) ((((long) wparam) >> 0x10) & 0xffff);
        private static ushort HIWORD(uint wparam) => (ushort) (wparam >> 0x10);
        private static int LOWORD(IntPtr wparam) => unchecked((short) (long) wparam);
        private static int GET_WHEEL_DELTA_WPARAM(IntPtr wparam) => (short) HIWORD(wparam);
        private static int GET_WHEEL_DELTA_WPARAM(uint wparam) => (short) HIWORD(wparam);
        private static int TOINT32(IntPtr param) => unchecked((int) (long) param);

        private const string ClassName = "GameWindow";
        private static readonly IntPtr TRUE = new IntPtr(1);

        private enum VK
        {
            LBUTTON = 0x01,
            RBUTTON = 0x02,
            CANCEL = 0x03,
            MBUTTON = 0x04,
            XBUTTON1 = 0x05,
            XBUTTON2 = 0x06,
            BACK = 0x08,
            TAB = 0x09,
            CLEAR = 0x0C,
            RETURN = 0x0D,
            SHIFT = 0x10,
            CONTROL = 0x11,
            MENU = 0x12,
            PAUSE = 0x13,
            CAPITAL = 0x14,
            KANA = 0x15,
            HANGUEL = 0x15,
            HANGUL = 0x15,
            JUNJA = 0x17,
            FINAL = 0x18,
            HANJA = 0x19,
            KANJI = 0x19,
            ESCAPE = 0x1B,
            CONVERT = 0x1C,
            NONCONVERT = 0x1D,
            ACCEPT = 0x1E,
            MODECHANGE = 0x1F,
            SPACE = 0x20,
            PRIOR = 0x21,
            NEXT = 0x22,
            END = 0x23,
            HOME = 0x24,
            LEFT = 0x25,
            UP = 0x26,
            RIGHT = 0x27,
            DOWN = 0x28,
            SELECT = 0x29,
            PRINT = 0x2A,
            EXECUTE = 0x2B,
            SNAPSHOT = 0x2C,
            INSERT = 0x2D,
            DELETE = 0x2E,
            HELP = 0x2F,
            LWIN = 0x5B,
            RWIN = 0x5C,
            APPS = 0x5D,
            SLEEP = 0x5F,
            NUMPAD0 = 0x60,
            NUMPAD1 = 0x61,
            NUMPAD2 = 0x62,
            NUMPAD3 = 0x63,
            NUMPAD4 = 0x64,
            NUMPAD5 = 0x65,
            NUMPAD6 = 0x66,
            NUMPAD7 = 0x67,
            NUMPAD8 = 0x68,
            NUMPAD9 = 0x69,
            MULTIPLY = 0x6A,
            ADD = 0x6B,
            SEPARATOR = 0x6C,
            SUBTRACT = 0x6D,
            DECIMAL = 0x6E,
            DIVIDE = 0x6F,
            F1 = 0x70,
            F2 = 0x71,
            F3 = 0x72,
            F4 = 0x73,
            F5 = 0x74,
            F6 = 0x75,
            F7 = 0x76,
            F8 = 0x77,
            F9 = 0x78,
            F10 = 0x79,
            F11 = 0x7A,
            F12 = 0x7B,
            F13 = 0x7C,
            F14 = 0x7D,
            F15 = 0x7E,
            F16 = 0x7F,
            F17 = 0x80,
            F18 = 0x81,
            F19 = 0x82,
            F20 = 0x83,
            F21 = 0x84,
            F22 = 0x85,
            F23 = 0x86,
            F24 = 0x87,
            NUMLOCK = 0x90,
            SCROLL = 0x91,
            LSHIFT = 0xA0,
            RSHIFT = 0xA1,
            LCONTROL = 0xA2,
            RCONTROL = 0xA3,
            LMENU = 0xA4,
            RMENU = 0xA5,
            BROWSER_BACK = 0xA6,
            BROWSER_FORWARD = 0xA7,
            BROWSER_REFRESH = 0xA8,
            BROWSER_STOP = 0xA9,
            BROWSER_SEARCH = 0xAA,
            BROWSER_FAVORITES = 0xAB,
            BROWSER_HOME = 0xAC,
            VOLUME_MUTE = 0xAD,
            VOLUME_DOWN = 0xAE,
            VOLUME_UP = 0xAF,
            MEDIA_NEXT_TRACK = 0xB0,
            MEDIA_PREV_TRACK = 0xB1,
            MEDIA_STOP = 0xB2,
            MEDIA_PLAY_PAUSE = 0xB3,
            LAUNCH_MAIL = 0xB4,
            LAUNCH_MEDIA_SELECT = 0xB5,
            LAUNCH_APP1 = 0xB6,
            LAUNCH_APP2 = 0xB7,
            OEM_1 = 0xBA,
            OEM_PLUS = 0xBB,
            OEM_COMMA = 0xBC,
            OEM_MINUS = 0xBD,
            OEM_PERIOD = 0xBE,
            OEM_2 = 0xBF,
            OEM_3 = 0xC0,
            OEM_4 = 0xDB,
            OEM_5 = 0xDC,
            OEM_6 = 0xDD,
            OEM_7 = 0xDE,
            OEM_8 = 0xDF,
            OEM_102 = 0xE2,
            PROCESSKEY = 0xE5,
            PACKET = 0xE7,
            ATTN = 0xF6,
            CRSEL = 0xF7,
            EXSEL = 0xF8,
            EREOF = 0xF9,
            PLAY = 0xFA,
            ZOOM = 0xFB,
            NONAME = 0xFC,
            PA1 = 0xFD,
            OEM_CLEAR = 0xFE
        }

        private KeyCode VKToKeyCode(VK vk)
        {
            if ((int) vk > 0x30 && (int) vk < 0x5A)
            {
                return (int) vk switch
                {
                    'A' => KeyCode.A,
                    'B' => KeyCode.B,
                    'C' => KeyCode.C,
                    'D' => KeyCode.D,
                    'E' => KeyCode.E,
                    'F' => KeyCode.F,
                    'G' => KeyCode.G,
                    'H' => KeyCode.H,
                    'I' => KeyCode.I,
                    'J' => KeyCode.J,
                    'K' => KeyCode.K,
                    'L' => KeyCode.L,
                    'M' => KeyCode.M,
                    'N' => KeyCode.N,
                    'O' => KeyCode.O,
                    'P' => KeyCode.P,
                    'Q' => KeyCode.Q,
                    'R' => KeyCode.R,
                    'S' => KeyCode.S,
                    'T' => KeyCode.T,
                    'U' => KeyCode.U,
                    'V' => KeyCode.V,
                    'W' => KeyCode.W,
                    'X' => KeyCode.X,
                    'Y' => KeyCode.Y,
                    'Z' => KeyCode.Z,
                    '0' => KeyCode.Num0,
                    '1' => KeyCode.Num1,
                    '2' => KeyCode.Num2,
                    '3' => KeyCode.Num3,
                    '4' => KeyCode.Num4,
                    '5' => KeyCode.Num5,
                    '6' => KeyCode.Num6,
                    '7' => KeyCode.Num7,
                    '8' => KeyCode.Num8,
                    '9' => KeyCode.Num9,
                    _ => KeyCode.Unknown
                };
            }

            return vk switch
            {
                VK.ESCAPE => KeyCode.Escape,
                VK.LCONTROL => KeyCode.LControl,
                VK.LSHIFT => KeyCode.LShift,
                VK.LMENU => KeyCode.LAlt,
                VK.LWIN => KeyCode.LSystem,
                VK.RCONTROL => KeyCode.RControl,
                VK.RSHIFT => KeyCode.RShift,
                VK.RMENU => KeyCode.RAlt,
                VK.RWIN => KeyCode.RSystem,
                VK.APPS => KeyCode.Menu,
                VK.OEM_4 => KeyCode.LBracket,
                VK.OEM_6 => KeyCode.RBracket,
                VK.OEM_1 => KeyCode.Semicolon,
                VK.OEM_COMMA => KeyCode.Comma,
                VK.OEM_PERIOD => KeyCode.Period,
                VK.OEM_7 => KeyCode.Quote,
                VK.OEM_2 => KeyCode.Slash,
                VK.OEM_5 => KeyCode.Backslash,
                VK.OEM_3 => KeyCode.Tilde,
                VK.OEM_PLUS => KeyCode.Equal,
                VK.OEM_MINUS => KeyCode.Hyphen,
                VK.SPACE => KeyCode.Space,
                VK.RETURN => KeyCode.Enter,
                VK.BACK => KeyCode.Backspace,
                VK.TAB => KeyCode.Tab,
                VK.PRIOR => KeyCode.PageUp,
                VK.NEXT => KeyCode.PageDown,
                VK.END => KeyCode.End,
                VK.HOME => KeyCode.Home,
                VK.INSERT => KeyCode.Insert,
                VK.DELETE => KeyCode.Delete,
                VK.ADD => KeyCode.Add,
                VK.SUBTRACT => KeyCode.Subtract,
                VK.MULTIPLY => KeyCode.Multiply,
                VK.DIVIDE => KeyCode.Divide,
                VK.LEFT => KeyCode.Left,
                VK.RIGHT => KeyCode.Right,
                VK.UP => KeyCode.Up,
                VK.DOWN => KeyCode.Down,
                VK.NUMPAD0 => KeyCode.Numpad0,
                VK.NUMPAD1 => KeyCode.Numpad1,
                VK.NUMPAD2 => KeyCode.Numpad2,
                VK.NUMPAD3 => KeyCode.Numpad3,
                VK.NUMPAD4 => KeyCode.Numpad4,
                VK.NUMPAD5 => KeyCode.Numpad5,
                VK.NUMPAD6 => KeyCode.Numpad6,
                VK.NUMPAD7 => KeyCode.Numpad7,
                VK.NUMPAD8 => KeyCode.Numpad8,
                VK.NUMPAD9 => KeyCode.Numpad9,
                VK.F1 => KeyCode.F1,
                VK.F2 => KeyCode.F2,
                VK.F3 => KeyCode.F3,
                VK.F4 => KeyCode.F4,
                VK.F5 => KeyCode.F5,
                VK.F6 => KeyCode.F6,
                VK.F7 => KeyCode.F7,
                VK.F8 => KeyCode.F8,
                VK.F9 => KeyCode.F9,
                VK.F10 => KeyCode.F10,
                VK.F11 => KeyCode.F11,
                VK.F12 => KeyCode.F12,
                VK.F13 => KeyCode.F13,
                VK.F14 => KeyCode.F14,
                VK.F15 => KeyCode.F15,
                VK.PAUSE => KeyCode.Pause,
                _ => KeyCode.Unknown
            };
        }
    }
}