using System;
using System.Collections.Generic;
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
        private Scene _scene;
        const int count = 15;
        private List<Entity> _entities = new List<Entity>(count);
        private Texture texture;

        public void OnAttach(Batch batch)
        {
            _scene = new Scene(Application.Instance.SceneRenderer);
            batch.Sumbit((() =>
                {
                    _viewportPane = new ViewportPane("Viewport", SceneRenderer.Main);
                    _viewportPane.Resized += (sender, args) => OnResize(args.Width, args.Height);


                    for (var i = 0; i < count; i++)
                    {
                        var newEntity = _scene.CreateEntity();
                        _entities.Add(newEntity);
                        newEntity.AddComponent<Component.Transform>().Matrix *= Matrix4x4.CreateTranslation(
                            (float) (Math.Sin(i * 2.0f * Math.PI / count) * 20.0f),
                            0.0f,
                            (float) (Math.Cos(i * 2.0f * Math.PI / count) * 20.0f)
                        );
                        newEntity.AddComponent<Component.Mesh>().Handle = Mesh.Create("bunny");
                    }

                    texture = ResourceLoader.LoadTexture("uffizi.dds");
                }),
                "Creating resources");
        }

        public void OnDetach()
        {
        }

        public void OnUpdate()
        {
            var dt = Global.Clock.Frame;


            // write some debug text
            Bgfx.DebugTextClear();
            Bgfx.DebugTextWrite(0, 5, DebugColor.White, DebugColor.Cyan, "Frame: {0:F3} ms", dt.AsSeconds() * 1000);
            // Bgfx.DebugTextWrite(0, 4, DebugColor.White, DebugColor.Cyan, "Size: {0}:{1}", _viewportPane.ViewportSize.X,
            //     _viewportPane.ViewportSize.Y);

            var transform = Matrix4x4.CreateFromYawPitchRoll(Global.Clock.TotalTime.AsSeconds() + 5 * 0.21f,
                Global.Clock.TotalTime.AsSeconds() + 5 * 0.37f, 0.0f);

            // transform.M41 = -15.0f * 5.0f;
            // transform.M42 = -15.0f * 5.0f;
            // transform.M43 = 0.0f;
            // model matrix
            // _entity.GetComponent<Component.Transform>().Matrix = transform;

            for (var i = 0; i < count; i++)
            {
                _entities[i].GetComponent<Component.Transform>().Matrix = Matrix4x4.CreateTranslation(
                    (float) (Math.Sin(i * 2.0f * Math.PI / count) * 5.0f),
                    (float) Math.Sin(i * 2.0f * Math.PI * Global.Clock.TotalTime.AsSeconds() / count) * 10.0f,
                    (float) (Math.Cos(i * 2.0f * Math.PI / count) * 5.0f)
                );
            }


            _scene.OnUpdate();
            _scene.OnRender();
        }

        private IntPtr handle;

        public void OnGuiRender()
        {
            ImGui.ShowDemoWindow();
            _viewportPane.OnGuiRender();

            handle = Gui.AddTexture(texture);

            ImGui.Begin("test");
            ImGui.Image(handle, Vector2.One * 1000.0f, Vector2.Zero, Vector2.One);
            ImGui.End();
        }

        public void OnResize(uint width, uint height)
        {
            _scene.OnResize(width, height);
        }

        private enum PrebuiltRenderState
        {
            StencilReflectionCraftStencil,
            StencilReflectionDrawReflected,
            StencilReflectionBlendPlane,
            StencilReflectionDrawScene,
            ProjectionShadowsDrawAmbient,
            ProjectionShadowsCraftStencil,
            ProjectionShadowsDrawDiffuse,
            CustomBlendLightTexture,
            CustomDrawPlaneBottom
        }

        private static readonly Dictionary<PrebuiltRenderState, RenderStateGroup> StateGroups =
            new Dictionary<PrebuiltRenderState, RenderStateGroup>
            {
                {
                    PrebuiltRenderState.StencilReflectionCraftStencil, new RenderStateGroup(
                        RenderState.WriteZ |
                        RenderState.DepthTestLess |
                        RenderState.Multisampling |
                        RenderState.WriteRGB,
                        uint.MaxValue,
                        StencilFlags.TestAlways |
                        StencilFlags.ReferenceValue(1) |
                        StencilFlags.ReadMask(0xff) |
                        StencilFlags.FailSReplace |
                        StencilFlags.FailZReplace |
                        StencilFlags.PassZReplace,
                        StencilFlags.None)
                },
                {
                    PrebuiltRenderState.StencilReflectionDrawReflected, new RenderStateGroup(
                        RenderState.WriteRGB |
                        RenderState.WriteA |
                        RenderState.WriteZ |
                        RenderState.DepthTestLess |
                        RenderState.CullCounterclockwise |
                        RenderState.Multisampling |
                        RenderState.BlendAlpha,
                        uint.MaxValue,
                        StencilFlags.TestEqual |
                        StencilFlags.ReferenceValue(1) |
                        StencilFlags.ReadMask(1) |
                        StencilFlags.FailSKeep |
                        StencilFlags.FailZKeep |
                        StencilFlags.PassZKeep,
                        StencilFlags.None)
                },
                {
                    PrebuiltRenderState.StencilReflectionBlendPlane, new RenderStateGroup(
                        RenderState.WriteRGB |
                        RenderState.WriteZ |
                        RenderState.DepthTestLess |
                        RenderState.BlendFunction(RenderState.BlendOne, RenderState.BlendSourceColor) |
                        RenderState.CullClockwise |
                        RenderState.Multisampling,
                        uint.MaxValue,
                        StencilFlags.None,
                        StencilFlags.None)
                },
                {
                    PrebuiltRenderState.StencilReflectionDrawScene, new RenderStateGroup(
                        RenderState.WriteRGB |
                        RenderState.WriteZ |
                        RenderState.DepthTestLess |
                        RenderState.CullClockwise |
                        RenderState.Multisampling,
                        uint.MaxValue,
                        StencilFlags.None,
                        StencilFlags.None)
                },
                {
                    PrebuiltRenderState.CustomBlendLightTexture, new RenderStateGroup(
                        RenderState.WriteRGB |
                        RenderState.WriteA |
                        RenderState.WriteZ |
                        RenderState.DepthTestLess |
                        RenderState.CullClockwise |
                        RenderState.Multisampling |
                        RenderState.BlendFunction(RenderState.BlendSourceColor, RenderState.BlendInverseSourceColor),
                        uint.MaxValue,
                        StencilFlags.None,
                        StencilFlags.None)
                }
            };
    }
}