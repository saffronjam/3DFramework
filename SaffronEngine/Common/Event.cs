using System;
using System.Runtime.InteropServices;

namespace SaffronEngine.Common
{
    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Enumeration of the different types of events
    /// </summary>
    ////////////////////////////////////////////////////////////
    public enum EventType
    {
        /// <summary>Event triggered when a window is manually closed</summary>
        Closed,

        /// <summary>Event triggered when a window is resized</summary>
        Resized,

        /// <summary>Event triggered when a window loses the focus</summary>
        LostFocus,

        /// <summary>Event triggered when a window gains the focus</summary>
        GainedFocus,

        /// <summary>Event triggered when a valid character is entered</summary>
        TextEntered,

        /// <summary>Event triggered when a keyboard key is pressed</summary>
        KeyPressed,

        /// <summary>Event triggered when a keyboard key is released</summary>
        KeyReleased,

        /// <summary>Event triggered when a mouse wheel is scrolled</summary>
        MouseWheelScrolled,

        /// <summary>Event triggered when a mouse button is pressed</summary>
        MouseButtonPressed,

        /// <summary>Event triggered when a mouse button is released</summary>
        MouseButtonReleased,

        /// <summary>Event triggered when the mouse moves within the area of a window</summary>
        MouseMoved,

        /// <summary>Event triggered when the mouse enters the area of a window</summary>
        MouseEntered,

        /// <summary>Event triggered when the mouse leaves the area of a window</summary>
        MouseLeft,

        /// <summary>Event triggered when a touch begins</summary>
        TouchBegan,

        /// <summary>Event triggered when a touch is moved</summary>
        TouchMoved,

        /// <summary>Event triggered when a touch is ended</summary>
        TouchEnded,
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Keyboard event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct KeyEvent
    {
        /// <summary>Code of the key (see KeyCode enum)</summary>
        public KeyCode Code;

        /// <summary>Is the Alt modifier pressed?</summary>
        public int Alt;

        /// <summary>Is the Control modifier pressed?</summary>
        public int Control;

        /// <summary>Is the Shift modifier pressed?</summary>
        public int Shift;

        /// <summary>Is the System modifier pressed?</summary>
        public int System;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Text event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct TextEvent
    {
        /// <summary>UTF-32 value of the character</summary>
        public uint Unicode;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Mouse move event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct MouseMoveEvent
    {
        /// <summary>X coordinate of the mouse cursor</summary>
        public int X;

        /// <summary>Y coordinate of the mouse cursor</summary>
        public int Y;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Mouse buttons event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct MouseButtonEvent
    {
        /// <summary>Code of the button (see MouseButton enum)</summary>
        public MouseButtonCode Button;

        /// <summary>X coordinate of the mouse cursor</summary>
        public int X;

        /// <summary>Y coordinate of the mouse cursor</summary>
        public int Y;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Mouse wheel scroll event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct MouseWheelScrollEvent
    {
        /// <summary>Mouse Wheel which triggered the event</summary>
        public MouseWheelCode Wheel;

        /// <summary>Scroll amount</summary>
        public float Delta;

        /// <summary>X coordinate of the mouse cursor</summary>
        public int X;

        /// <summary>Y coordinate of the mouse cursor</summary>
        public int Y;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Size event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct SizeEvent
    {
        /// <summary>New width of the window</summary>
        public uint Width;

        /// <summary>New height of the window</summary>
        public uint Height;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Touch event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct TouchEvent
    {
        /// <summary>Index of the finger in case of multi-touch events</summary>
        public uint Finger;

        /// <summary>X position of the touch, relative to the left of the owner window</summary>
        public int X;

        /// <summary>Y position of the touch, relative to the top of the owner window</summary>
        public int Y;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Event defines a system event and its parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Explicit, Size = 20)]
    public struct Event
    {
        /// <summary>Type of event (see EventType enum)</summary>
        [FieldOffset(0)]
        public EventType Type;

        /// <summary>Arguments for size events (Resized)</summary>
        [FieldOffset(4)]
        public SizeEvent Size;

        /// <summary>Arguments for key events (KeyPressed, KeyReleased)</summary>
        [FieldOffset(4)]
        public KeyEvent Key;

        /// <summary>Arguments for text events (TextEntered)</summary>
        [FieldOffset(4)]
        public TextEvent Text;

        /// <summary>Arguments for mouse move events (MouseMoved)</summary>
        [FieldOffset(4)]
        public MouseMoveEvent MouseMove;

        /// <summary>Arguments for mouse button events (MouseButtonPressed, MouseButtonReleased)</summary>
        [FieldOffset(4)]
        public MouseButtonEvent MouseButton;

        /// <summary>Arguments for mouse wheel scroll events (MouseWheelScrolled)</summary>
        [FieldOffset(4)]
        public MouseWheelScrollEvent MouseWheelScroll;

        /// <summary>Arguments for touch events (TouchBegan, TouchMoved, TouchEnded)</summary>
        [FieldOffset(4)]
        public TouchEvent Touch;
    }
}
