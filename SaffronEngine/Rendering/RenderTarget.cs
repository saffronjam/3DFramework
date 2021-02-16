using System;
using System.Numerics;
using Renderer;
using SaffronEngine.Common;
using Serilog;

namespace SaffronEngine.Rendering
{
    public class RenderTarget
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

        private FrameBuffer _frameBuffer;

        public FrameBuffer FrameBuffer
        {
            get => _frameBuffer;
            set
            {
                _frameBuffer = value;
                NewFrameBuffer?.Invoke(this, EventArgs.Empty);
            }
        }

        public bool AwaitingResize { get; private set; }

        public void Resize()
        {
            var fbTextures = new[]
            {
                Texture.Create2D((int) Width, (int) Height, false,
                    1, TextureFormat.RGBA8, TextureFlags.RenderTarget),
                Texture.Create2D((int) Width, (int) Height, false,
                    1, TextureFormat.D24S8, TextureFlags.RenderTarget),
            };
            FrameBuffer = new FrameBuffer(fbTextures, true);
            
            
            // FrameBuffer = new FrameBuffer((int) Width, (int) Height, TextureFormat.RGBA8,
            //     TextureFlags.RenderTarget | TextureFlags.ClampU | TextureFlags.ClampV);
            AwaitingResize = false;

            var sizeEvent = new SizeEvent {Height = Height, Width = Width};
            Resized?.Invoke(this, new SizeEventArgs(sizeEvent));
        }

        /// <summary>Event handler for the Resized event</summary>
        public event EventHandler<SizeEventArgs> Resized = null;

        public event EventHandler NewFrameBuffer = null;
    }
}