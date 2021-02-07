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
        private Entity _entity;

        public void OnAttach(Batch batch)
        {
            batch.Sumbit((() =>
                {
                    _scene = new Scene(Application.Instance.SceneRenderer);
                    _viewportPane = new ViewportPane("Viewport", SceneRenderer.Main);
                    _viewportPane.Resized += (sender, args) => OnResize(args.Width, args.Height);

                    _entity = _scene.CreateEntity();

                    _entity.AddComponent<Component.Mesh>().Handle = Mesh.Create("bunny");
                    _entity.AddComponent<Component.Transform>();
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
            Bgfx.DebugTextWrite(0, 1, DebugColor.White, DebugColor.Blue, "SharpBgfx/Samples/01-Cubes");
            Bgfx.DebugTextWrite(0, 2, DebugColor.White, DebugColor.Cyan, "Description: Rendering simple static mesh.");
            Bgfx.DebugTextWrite(0, 3, DebugColor.White, DebugColor.Cyan, "Frame: {0:F3} ms", dt.AsSeconds() * 1000);
            Bgfx.DebugTextWrite(0, 4, DebugColor.White, DebugColor.Cyan, "Size: {0}:{1}", _viewportPane.ViewportSize.X,
                _viewportPane.ViewportSize.Y);
            
            var transform = Matrix4x4.CreateFromYawPitchRoll(Global.Clock.TotalTime.AsSeconds() + 5 * 0.21f,
                Global.Clock.TotalTime.AsSeconds() + 5 * 0.37f, 0.0f);
            
            // transform.M41 = -15.0f * 5.0f;
            // transform.M42 = -15.0f * 5.0f;
            // transform.M43 = 0.0f;
            // model matrix
            // _entity.GetComponent<Component.Transform>().Matrix = transform;


            _scene.OnUpdate();
            _scene.OnRender();
        }

        public void OnGuiRender()
        {
            ImGui.ShowDemoWindow();
            _viewportPane.OnGuiRender();
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