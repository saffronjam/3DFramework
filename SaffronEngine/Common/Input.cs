using System;
using System.Collections;
using System.Numerics;

namespace SaffronEngine.Common
{
    public static class Input
    {
        private static readonly BitArray KeyboardState = new BitArray((int) KeyCode.Count);
        private static readonly BitArray LastKeyboardState = new BitArray((int) KeyCode.Count);
        private static readonly BitArray MouseState = new BitArray((int) MouseButtonCode.Count);
        private static readonly BitArray LastMouseState = new BitArray((int) MouseButtonCode.Count);


        private static Vector2 _mousePosition = new Vector2(0, 0);
        private static Vector2 _lastMousePosition = new Vector2(0, 0);
        private static bool _isMouseInWindow = false;

        public static void AddEventSource(Window eventSource)
        {
            eventSource.KeyPressed += OnKeyPressed;
            eventSource.KeyReleased += OnKeyReleased;
            eventSource.MouseButtonPressed += OnMouseButtonPressed;
            eventSource.MouseButtonReleased += OnMouseButtonReleased;
            eventSource.MouseMoved += OnMouseMoved;
            eventSource.MouseWheelScrolled += OnMouseWheelScrolled;
            eventSource.MouseEntered += OnMouseEntered;
            eventSource.MouseLeft += OnMouseLeft;
        }

        public static void RemoveEventSource(Window eventSource)
        {
            eventSource.KeyPressed -= OnKeyPressed;
            eventSource.KeyReleased -= OnKeyReleased;
            eventSource.MouseButtonPressed -= OnMouseButtonPressed;
            eventSource.MouseButtonReleased -= OnMouseButtonReleased;
            eventSource.MouseMoved -= OnMouseMoved;
            eventSource.MouseWheelScrolled -= OnMouseWheelScrolled;
            eventSource.MouseEntered -= OnMouseEntered;
            eventSource.MouseLeft -= OnMouseLeft;
        }

        public static void OnUpdate()
        {
            for (var i = 0; i < KeyboardState.Count; i++)
            {
                LastKeyboardState[i] = KeyboardState[i];
            }

            for (var i = 0; i < MouseState.Count; i++)
            {
                LastMouseState[i] = MouseState[i];
            }

            _lastMousePosition = _mousePosition;
            VerticalScroll = 0.0f;
            HorizontalScroll = 0.0f;
        }

        public static bool IsKeyDown(KeyCode key)
        {
            return KeyboardState[(int) key];
        }

        public static bool IsKeyUp(KeyCode key)
        {
            return !KeyboardState[(int) key];
        }

        public static bool IsKeyPressed(KeyCode key)
        {
            return KeyboardState[(int) key] && !LastKeyboardState[(int) key];
        }

        public static bool IsKeyReleased(KeyCode key)
        {
            return !KeyboardState[(int) key] && LastKeyboardState[(int) key];
        }

        public static bool IsMouseButtonDown(MouseButtonCode mouseButton)
        {
            return MouseState[(int) mouseButton];
        }

        public static bool IsMouseButtonUp(MouseButtonCode mouseButton)
        {
            return !MouseState[(int) mouseButton];
        }

        public static bool IsMouseButtonPressed(MouseButtonCode mouseButton)
        {
            return MouseState[(int) mouseButton] && !LastMouseState[(int) mouseButton];
        }

        public static bool IsMouseButtonReleased(MouseButtonCode mouseButton)
        {
            return !MouseState[(int) mouseButton] && LastMouseState[(int) mouseButton];
        }

        public static bool IsMouseInWindow()
        {
            return _isMouseInWindow;
        }

        public static Vector2 MousePosition
        {
            get => _mousePosition;
            private set => _mousePosition = value;
        }

        public static Vector2 MouseSwipe => MousePosition - _lastMousePosition;

        public static float VerticalScroll { get; set; }
        public static float HorizontalScroll { get; set; }


        // Event handlers

        private static void OnKeyPressed(object sender, KeyEventArgs args)
        {
            KeyboardState[(int) args.Code] = true;
        }

        private static void OnKeyReleased(object sender, KeyEventArgs args)
        {
            KeyboardState[(int) args.Code] = false;
        }

        private static void OnMouseButtonPressed(object sender, MouseButtonEventArgs args)
        {
            MouseState[(int) args.Button] = true;
        }

        private static void OnMouseButtonReleased(object sender, MouseButtonEventArgs args)
        {
            MouseState[(int) args.Button] = false;
        }

        private static void OnMouseMoved(object sender, MouseMoveEventArgs args)
        {
            _lastMousePosition = _mousePosition;
            _mousePosition.X = args.X;
            _mousePosition.Y = args.Y;
        }

        private static void OnMouseWheelScrolled(object sender, MouseWheelScrollEventArgs args)
        {
            switch (args.Wheel)
            {
                case MouseWheelCode.HorizontalWheel:
                    HorizontalScroll = args.Delta;
                    break;
                case MouseWheelCode.VerticalWheel:
                    VerticalScroll = args.Delta;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            _mousePosition.X = args.X;
            _mousePosition.Y = args.Y;
        }

        private static void OnMouseEntered(object sender, EventArgs e)
        {
            _isMouseInWindow = true;
        }

        private static void OnMouseLeft(object sender, EventArgs e)
        {
            _isMouseInWindow = false;
        }
    }
}