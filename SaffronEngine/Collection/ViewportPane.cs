using System;
using System.Numerics;
using System.Text;
using ImGuiNET;
using Renderer;
using SaffronEngine.Common;
using SaffronEngine.Rendering;
using Serilog;

namespace SaffronEngine.Collection
{
    public class ViewportPane
    {
        private readonly string _viewportTitle;
        private uint _dockId;
        public SceneRenderer.Target Target { get; set; }
        private IntPtr _viewportGuiTextureHandle;

        private Vector2 _topLeft;
        private Vector2 _bottomRight;

        private readonly uint _inactiveBorderColor = BitConverter.ToUInt32(new byte[] {255, 140, 0, 80});
        private readonly uint _activeBorderColor = BitConverter.ToUInt32(new byte[] {255, 140, 0, 180});

        public ViewportPane(string viewportTitle, SceneRenderer.Target target)
        {
            this._viewportTitle = viewportTitle;

            _viewportGuiTextureHandle = Gui.AddTexture(target.FrameBuffer.GetTexture());
            Target = target;
            Target.Resized += (sender, args) =>
            {
                Gui.RemoveTexture(_viewportGuiTextureHandle);
                Gui.AddTexture(Target.FrameBuffer.GetTexture());
            };

            TopLeft = Vector2.Zero;
            BottomRight = Vector2.Zero;
        }

        public void OnGuiRender()
        {
            var tl = TopLeft;
            var br = BottomRight;

            ImGui.PushStyleVar(ImGuiStyleVar.WindowPadding, Vector2.Zero);

            const int uuid = 0;

            var builder = new StringBuilder();
            builder.Append(_viewportTitle).Append("##").Append(uuid);

            ImGui.Begin(builder.ToString(), ImGuiWindowFlags.NoFocusOnAppearing);

            _dockId = ImGui.GetWindowDockID();

            Hovered = ImGui.IsWindowHovered();
            Focused = ImGui.IsWindowFocused();

            var viewportOffset = ImGui.GetCursorPos(); // includes tab bar
            var minBound = ImGui.GetWindowPos();
            minBound.X += viewportOffset.X;
            minBound.Y += viewportOffset.Y;

            var windowSize = ImGui.GetWindowSize();
            var maxBound = new Vector2(minBound.X + windowSize.X - viewportOffset.X,
                minBound.Y + windowSize.Y - viewportOffset.Y);
            _topLeft.X = minBound.X;
            _topLeft.Y = minBound.Y;
            _bottomRight.X = maxBound.X;
            _bottomRight.Y = maxBound.Y;

            var vpSize = ViewportSize;
            var fbTexture = Target.FrameBuffer.GetTexture();
            var imageRendererId = fbTexture.GetHashCode();

            ImGui.Image((IntPtr) imageRendererId, new Vector2(vpSize.X, vpSize.Y));

            ImGui.GetWindowDrawList().AddRect(new Vector2(TopLeft.X, tl.Y), new Vector2(br.X, br.Y),
                Focused ? _activeBorderColor : _inactiveBorderColor, 0.0f, ImDrawCornerFlags.All, 4);

            ImGui.End();
            ImGui.PopStyleVar();


            if (!(Math.Abs(ViewportSize.X - Target.Width) > 0.5f) &&
                !(Math.Abs(ViewportSize.Y - Target.Height) > 0.5f)) return;

            Target.Width = (uint) ViewportSize.X;
            Target.Height = (uint) ViewportSize.Y;

            Resized?.Invoke(this,
                new SizeEventArgs(new SizeEvent {Width = (uint) ViewportSize.X, Height = (uint) ViewportSize.Y}));
        }

        public Vector2 TopLeft
        {
            get => _topLeft;
            private set => _topLeft = value;
        }

        public Vector2 BottomRight
        {
            get => _bottomRight;
            private set => _bottomRight = value;
        }

        public bool Hovered { get; private set; }
        public bool Focused { get; private set; }

        public Vector2 ViewportSize => BottomRight - TopLeft;

        public event EventHandler<SizeEventArgs> Resized = null;
    }
}