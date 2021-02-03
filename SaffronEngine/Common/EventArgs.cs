using System;

namespace SaffronEngine.Common
{
    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Keyboard event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    public class KeyEventArgs : EventArgs
    {
        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Construct the key arguments from a key event
        /// </summary>
        /// <param name="e">Key event</param>
        ////////////////////////////////////////////////////////////
        public KeyEventArgs(KeyEvent e)
        {
            Code = e.Code;
            Alt = e.Alt != 0;
            Control = e.Control != 0;
            Shift = e.Shift != 0;
            System = e.System != 0;
        }

        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Provide a string describing the object
        /// </summary>
        /// <returns>String description of the object</returns>
        ////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return "[KeyEventArgs]" +
                   " Code(" + Code + ")" +
                   " Alt(" + Alt + ")" +
                   " Control(" + Control + ")" +
                   " Shift(" + Shift + ")" +
                   " System(" + System + ")";
        }

        /// <summary>Code of the key (see KeyCode enum)</summary>
        public KeyCode Code;

        /// <summary>Is the Alt modifier pressed?</summary>
        public bool Alt;

        /// <summary>Is the Control modifier pressed?</summary>
        public bool Control;

        /// <summary>Is the Shift modifier pressed?</summary>
        public bool Shift;

        /// <summary>Is the System modifier pressed?</summary>
        public bool System;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Text event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    public class TextEventArgs : EventArgs
    {
        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Construct the text arguments from a text event
        /// </summary>
        /// <param name="e">Text event</param>
        ////////////////////////////////////////////////////////////
        public TextEventArgs(TextEvent e)
        {
            Unicode = Char.ConvertFromUtf32((int)e.Unicode);
        }

        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Provide a string describing the object
        /// </summary>
        /// <returns>String description of the object</returns>
        ////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return "[TextEventArgs]" +
                   " Unicode(" + Unicode + ")";
        }

        /// <summary>UTF-16 value of the character</summary>
        public string Unicode;
    }

    ////////////////////////////////////////////////////////////
    /// <summary>
    /// Mouse move event parameters
    /// </summary>
    ////////////////////////////////////////////////////////////
    public class MouseMoveEventArgs : EventArgs
    {
        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Construct the mouse move arguments from a mouse move event
        /// </summary>
        /// <param name="e">Mouse move event</param>
        ////////////////////////////////////////////////////////////
        public MouseMoveEventArgs(MouseMoveEvent e)
        {
            X = e.X;
            Y = e.Y;
        }

        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Provide a string describing the object
        /// </summary>
        /// <returns>String description of the object</returns>
        ////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return "[MouseMoveEventArgs]" +
                   " X(" + X + ")" +
                   " Y(" + Y + ")";
        }

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
    public class MouseButtonEventArgs : EventArgs
    {
        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Construct the mouse button arguments from a mouse button event
        /// </summary>
        /// <param name="e">Mouse button event</param>
        ////////////////////////////////////////////////////////////
        public MouseButtonEventArgs(MouseButtonEvent e)
        {
            Button = e.Button;
            X = e.X;
            Y = e.Y;
        }

        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Provide a string describing the object
        /// </summary>
        /// <returns>String description of the object</returns>
        ////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return "[MouseButtonEventArgs]" +
                   " Button(" + Button + ")" +
                   " X(" + X + ")" +
                   " Y(" + Y + ")";
        }

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
    public class MouseWheelScrollEventArgs : EventArgs
    {
        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Construct the mouse wheel scroll arguments from a mouse wheel scroll event
        /// </summary>
        /// <param name="e">Mouse wheel scroll event</param>
        ////////////////////////////////////////////////////////////
        public MouseWheelScrollEventArgs(MouseWheelScrollEvent e)
        {
            Delta = e.Delta;
            Wheel = e.Wheel;
            X = e.X;
            Y = e.Y;
        }

        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Provide a string describing the object
        /// </summary>
        /// <returns>String description of the object</returns>
        ////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return "[MouseWheelScrollEventArgs]" +
                   " Wheel(" + Wheel + ")" +
                   " Delta(" + Delta + ")" +
                   " X(" + X + ")" +
                   " Y(" + Y + ")";
        }

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
    public class SizeEventArgs : EventArgs
    {
        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Construct the size arguments from a size event
        /// </summary>
        /// <param name="e">Size event</param>
        ////////////////////////////////////////////////////////////
        public SizeEventArgs(SizeEvent e)
        {
            Width = e.Width;
            Height = e.Height;
        }

        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Provide a string describing the object
        /// </summary>
        /// <returns>String description of the object</returns>
        ////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return "[SizeEventArgs]" +
                   " Width(" + Width + ")" +
                   " Height(" + Height + ")";
        }

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
    public class TouchEventArgs : EventArgs
    {
        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Construct the touch arguments from a touch event
        /// </summary>
        /// <param name="e">Touch event</param>
        ////////////////////////////////////////////////////////////
        public TouchEventArgs(TouchEvent e)
        {
            Finger = e.Finger;
            X = e.X;
            Y = e.Y;
        }

        ////////////////////////////////////////////////////////////
        /// <summary>
        /// Provide a string describing the object
        /// </summary>
        /// <returns>String description of the object</returns>
        ////////////////////////////////////////////////////////////
        public override string ToString()
        {
            return "[TouchEventArgs]" +
                   " Finger(" + Finger + ")" +
                   " X(" + X + ")" +
                   " Y(" + Y + ")";
        }

        /// <summary>Index of the finger in case of multi-touch events</summary>
        public uint Finger;

        /// <summary>X position of the touch, relative to the left of the owner window</summary>
        public int X;

        /// <summary>Y position of the touch, relative to the top of the owner window</summary>
        public int Y;
    }
}
