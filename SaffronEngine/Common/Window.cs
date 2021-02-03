using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Numerics;

namespace SaffronEngine.Common
{
    public abstract class Window
    {
        private readonly ConcurrentQueue<Event> _events = new ConcurrentQueue<Event>();

        private string _title;
        public uint Width { get; private set; }
        public uint Height { get; private set; }

        public Vector2 Size => new Vector2(Width, Height);
        public bool Focused { get; private set; }

        public virtual string Title
        {
            get => _title;
            set => _title = value;
        }

        public virtual IntPtr Handle => IntPtr.Zero;

        protected Window(string title, uint width, uint height)
        {
            Width = width;
            Height = height;
            _title = title;

            GainedFocus += (sender, args) => { Focused = true; };
            LostFocus += (sender, args) => { Focused = false; };
            Resized += (sender, args) =>
            {
                Width = args.Width;
                Height = args.Height;
            };
        }

        public abstract void Show();

        public abstract int RunMessageLoop();

        public abstract void Close();

        public void DispatchEvents()
        {
            while (_events.TryDequeue(out var e))
            {
                CallEventHandler(e);
            }
        }

        public void PushEvent(Event e)
        {
            _events.Enqueue(e);
        }

        private void CallEventHandler(Event e)
        {
            switch (e.Type)
            {
                case EventType.Closed:
                    Closed?.Invoke(this, EventArgs.Empty);
                    break;
                case EventType.GainedFocus:
                    GainedFocus?.Invoke(this, EventArgs.Empty);
                    break;
                case EventType.KeyPressed:
                    KeyPressed?.Invoke(this, new KeyEventArgs(e.Key));
                    break;
                case EventType.KeyReleased:
                    KeyReleased?.Invoke(this, new KeyEventArgs(e.Key));
                    break;
                case EventType.LostFocus:
                    LostFocus?.Invoke(this, EventArgs.Empty);
                    break;
                case EventType.MouseButtonPressed:
                    MouseButtonPressed?.Invoke(this, new MouseButtonEventArgs(e.MouseButton));
                    break;
                case EventType.MouseButtonReleased:
                    MouseButtonReleased?.Invoke(this, new MouseButtonEventArgs(e.MouseButton));
                    break;
                case EventType.MouseEntered:
                    MouseEntered?.Invoke(this, EventArgs.Empty);
                    break;
                case EventType.MouseLeft:
                    MouseLeft?.Invoke(this, EventArgs.Empty);
                    break;
                case EventType.MouseMoved:
                    MouseMoved?.Invoke(this, new MouseMoveEventArgs(e.MouseMove));
                    break;
                case EventType.MouseWheelScrolled:
                    MouseWheelScrolled?.Invoke(this, new MouseWheelScrollEventArgs(e.MouseWheelScroll));
                    break;
                case EventType.Resized:
                    Resized?.Invoke(this, new SizeEventArgs(e.Size));
                    break;
                case EventType.TextEntered:
                    TextEntered?.Invoke(this, new TextEventArgs(e.Text));
                    break;
                case EventType.TouchBegan:
                    TouchBegan?.Invoke(this, new TouchEventArgs(e.Touch));
                    break;
                case EventType.TouchMoved:
                    TouchMoved?.Invoke(this, new TouchEventArgs(e.Touch));
                    break;
                case EventType.TouchEnded:
                    TouchEnded?.Invoke(this, new TouchEventArgs(e.Touch));

                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }


        /// <summary>Event handler for the Closed event</summary>
        public event EventHandler Closed = null;

        /// <summary>Event handler for the Resized event</summary>
        public event EventHandler<SizeEventArgs> Resized = null;

        /// <summary>Event handler for the LostFocus event</summary>
        public event EventHandler LostFocus = null;

        /// <summary>Event handler for the GainedFocus event</summary>
        public event EventHandler GainedFocus = null;

        /// <summary>Event handler for the TextEntered event</summary>
        public event EventHandler<TextEventArgs> TextEntered = null;

        /// <summary>Event handler for the KeyPressed event</summary>
        public event EventHandler<KeyEventArgs> KeyPressed = null;

        /// <summary>Event handler for the KeyReleased event</summary>
        public event EventHandler<KeyEventArgs> KeyReleased = null;

        /// <summary>Event handler for the MouseWheelScrolled event</summary>
        public event EventHandler<MouseWheelScrollEventArgs> MouseWheelScrolled = null;

        /// <summary>Event handler for the MouseButtonPressed event</summary>
        public event EventHandler<MouseButtonEventArgs> MouseButtonPressed = null;

        /// <summary>Event handler for the MouseButtonReleased event</summary>
        public event EventHandler<MouseButtonEventArgs> MouseButtonReleased = null;

        /// <summary>Event handler for the MouseMoved event</summary>
        public event EventHandler<MouseMoveEventArgs> MouseMoved = null;

        /// <summary>Event handler for the MouseEntered event</summary>
        public event EventHandler MouseEntered = null;

        /// <summary>Event handler for the MouseLeft event</summary>
        public event EventHandler MouseLeft = null;

        /// <summary>Event handler for the TouchBegan event</summary>
        public event EventHandler<TouchEventArgs> TouchBegan = null;

        /// <summary>Event handler for the TouchMoved event</summary>
        public event EventHandler<TouchEventArgs> TouchMoved = null;

        /// <summary>Event handler for the TouchEnded event</summary>
        public event EventHandler<TouchEventArgs> TouchEnded = null;
    }
}