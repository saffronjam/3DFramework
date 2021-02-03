using System;
using System.Collections.Generic;
using System.Drawing;
using System.Numerics;
using Renderer;
using SaffronEngine.Common;

namespace SaffronEngine.Rendering
{
    public class SceneRenderer
    {
        public enum ViewID : ushort
        {
            Main = 0,
            Gui = 1
        }

        public class Target
        {
            private uint _width, _height;

            public uint Width
            {
                get => _width;
                set
                {
                    _width = value;
                    AwaitingResize = true;
                }
            }

            public uint Height
            {
                get => _height;
                set
                {
                    _height = value;
                    AwaitingResize = true;
                }
            }

            public Vector2 Size => new Vector2(Width, Height);

            public FrameBuffer FrameBuffer { get; set; }
            public bool AwaitingResize { get; private set; }

            public void Resize()
            {
                Main.FrameBuffer.Dispose();
                Main.FrameBuffer = new FrameBuffer((int) Main.Width, (int) Main.Height, TextureFormat.RGB8);
                AwaitingResize = false;

                var sizeEvent = new SizeEvent {Height = Height, Width = Width};
                Resized?.Invoke(this, new SizeEventArgs(sizeEvent));
            }

            /// <summary>Event handler for the Resized event</summary>
            public event EventHandler<SizeEventArgs> Resized = null;
        }

        private static SceneRenderer _inst;
        public static Target Main { get; private set; }

        private readonly List<Action> _scheduledRendering = new List<Action>();

        public Color ClearColor { get; set; }

        public SceneRenderer()
        {
            _inst = this;
            Main = new Target {FrameBuffer = new FrameBuffer(1, 1, TextureFormat.RGBA8)};
            Application.Instance.Window.Resized += (sender, args) => OnWindowResize(args.Width, args.Height);
        }

        public static void Sumbit(Action renderAction)
        {
            _inst._scheduledRendering.Add(renderAction);
        }

        public void Execute()
        {
            if (Main.AwaitingResize)
            {
                var scale = Application.Instance.Window.Size;
                Bgfx.SetViewRect((ushort) ViewID.Main, 0, 0, (int) Main.Width, (int) Main.Height);
                Main.Resize();
            }

            Bgfx.SetViewFrameBuffer((ushort) ViewID.Main, Main.FrameBuffer);
            var colorUint = (uint) (((ClearColor.R << 24) |
                                     (ClearColor.G << 16) |
                                     (ClearColor.B << 8) |
                                     ClearColor.A) & 0xffffffffL);

            foreach (var viewID in Enum.GetValues(typeof(ViewID)))
            {
                Bgfx.SetViewClear((ushort) viewID, ClearTargets.Color | ClearTargets.Depth, colorUint);
            }

            var viewMatrix = Matrix4x4.CreateLookAt(new Vector3(0.0f, 0.0f, -35.0f), Vector3.Zero, Vector3.UnitY);
            var projMatrix = Matrix4x4.CreatePerspectiveFieldOfView((float) Math.PI / 3,
                (float) Main.Width / Main.Height, 0.1f, 100.0f);
            unsafe
            {
                Bgfx.SetViewTransform(0, &viewMatrix.M11, &projMatrix.M11);
            }

            Bgfx.Touch(0);

            foreach (var action in _scheduledRendering)
            {
                action.Invoke();
            }

            _scheduledRendering.Clear();
        }

        public static void OnWindowResize(uint width, uint height)
        {
            Bgfx.Reset((int) width, (int) height, ResetFlags.Vsync);
        }
    }
}