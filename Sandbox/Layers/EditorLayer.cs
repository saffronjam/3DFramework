using System;
using System.Numerics;
using ImGuiNET;
using Renderer;
using SaffronEngine.Collection;
using SaffronEngine.Common;
using SaffronEngine.Rendering;
using Serilog;

namespace Sandbox.Layers
{
    public class EditorLayer : Layer
    {
        private ViewportPane _viewportPane;

        private VertexBuffer vbh;
        private IndexBuffer ibh;
        private Program program;
        private IntPtr image;


        public void OnAttach(Batch batch)
        {
            batch.Sumbit((() =>
            {
                _viewportPane = new ViewportPane("Viewport", SceneRenderer.Main);
                vbh = Cube.CreateVertexBuffer();
                ibh = Cube.CreateIndexBuffer();
                program = ResourceLoader.LoadProgram("vs_cubes", "fs_cubes");
            }), "Creating resources");
            image = Gui.AddTexture(ResourceLoader.LoadTexture("Saffron.dds"));
        }

        public void OnDetach()
        {
            vbh.Dispose();
            ibh.Dispose();
            program.Dispose();
        }

        public void OnUpdate()
        {
            var dt = Global.Clock.Frame;

            // write some debug text
            Bgfx.DebugTextClear();
            Bgfx.DebugTextWrite(0, 1, DebugColor.White, DebugColor.Blue, "SharpBgfx/Samples/01-Cubes");
            Bgfx.DebugTextWrite(0, 2, DebugColor.White, DebugColor.Cyan, "Description: Rendering simple static mesh.");
            Bgfx.DebugTextWrite(0, 3, DebugColor.White, DebugColor.Cyan, "Frame: {0:F3} ms", dt.AsSeconds() * 1000);
            Bgfx.DebugTextWrite(0, 4, DebugColor.White, DebugColor.Cyan, "Size: {0}:{1}", _viewportPane.ViewportSize.X,
                _viewportPane.ViewportSize.Y);

            SceneRenderer.Sumbit(() =>
            {
                // submit 11x11 cubes
                for (var y = 0; y < 11; y++)
                {
                    for (var x = 0; x < 11; x++)
                    {
                        // model matrix
                        var transform = Matrix4x4.CreateFromYawPitchRoll(Global.Clock.TotalTime.AsSeconds() + x * 0.21f,
                            Global.Clock.TotalTime.AsSeconds() + y * 0.37f, 0.0f);

                        transform.M41 = -15.0f + x * 3.0f;
                        transform.M42 = -15.0f + y * 3.0f;
                        transform.M43 = 0.0f;
                        unsafe
                        {
                            Bgfx.SetTransform(&transform.M11);
                        }

                        // set pipeline states
                        Bgfx.SetVertexBuffer(0, vbh);
                        Bgfx.SetIndexBuffer(ibh);
                        Bgfx.SetRenderState(RenderState.Default);

                        // submit primitives
                        Bgfx.Submit((ushort) SceneRenderer.ViewID.Main, program);
                    }
                }
            });
        }

        public void OnGuiRender()
        {
            ImGui.ShowDemoWindow();

            _viewportPane.OnGuiRender();
        }
    }
}