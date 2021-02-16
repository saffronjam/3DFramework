using System;
using System.Collections.Generic;
using System.Drawing;
using System.Numerics;
using System.Runtime.InteropServices;
using ImGuiNET;
using Renderer;
using SaffronEngine.Common;
using Serilog;

namespace SaffronEngine.Rendering
{
    public class SceneRenderer
    {
        private class RenderCommand
        {
            public Mesh Mesh { get; private set; }
            public Matrix4x4 Transform { get; private set; }

            public RenderCommand(Mesh mesh, Matrix4x4 transform)
            {
                Mesh = mesh;
                Transform = transform;
            }
        }


        public enum ViewID : ushort
        {
            BackBuffer = 0,
            ShadowMap0 = 1,
            ShadowMap1 = 2,
            ShadowMap2 = 3,
            ShadowMap3 = 4,
            ShadowMap4 = 5,
            VBlur0 = 6,
            HBlur0 = 7,
            VBlur1 = 8,
            HBlur1 = 9,
            VBlur2 = 10,
            HBlur2 = 11,
            VBlur3 = 12,
            HBlur3 = 13,
            DrawScene0 = 14,
            DrawScene1 = 15,
            DrawDepth0 = 16,
            DrawDepth1 = 17,
            DrawDepth2 = 18,
            DrawDepth3 = 19,
            Gui = 20
        }

        public struct CameraData
        {
            public Matrix4x4 Projection;
            public Matrix4x4 View;

            public CameraData(Matrix4x4 projection, Matrix4x4 view)
            {
                Projection = projection;
                View = view;
            }
        }


        private static SceneRenderer _inst;
        public static RenderTarget Main { get; private set; }

        private readonly ShadowMapManager _shadowMapManager;
        private readonly SmSettings _currentSmSettings;
        private int _shadowMapSize;

        private readonly List<Action> _scheduledRendering = new List<Action>();
        private readonly List<RenderCommand> _renderCommands = new List<RenderCommand>();

        // Mesh
        private readonly Program _meshProgram;

        // Skybox
        private readonly Program _skyboxProgram;
        private Texture _skyboxTexture;
        private Uniform _textureCubeUniform;
        private Uniform _skyboxTransform;

        private Uniform _textureSampler;
        private readonly Uniform[] _shadowMapSamplers = new Uniform[4];

        public ClearValues ClearValues { get; set; }

        private Scene _activeScene = null;
        public CameraData Camera { get; set; }


        // ----------------------------
        //         TEMPORARY
        // ----------------------------

        private float lightsTotalTime = 0.0f;
        private float sceneTotalTime = 0.0f;

        Texture _texFigure;
        Texture _texFlare;
        Texture _texFieldstone;

        Mesh _bunnyMesh;
        Mesh _treeMesh;
        Mesh _cubeMesh;
        Mesh _hollowcubeMesh;
        Mesh _hplaneMesh;
        Mesh _vplaneMesh;

        static float _texcoord = 5.0f;

        static PosNormalTexcoordVertex[] _HPlaneVertices =
        {
            new PosNormalTexcoordVertex(-1.0f, 0.0f, 1.0f, GenUtils.PackF4u(0.0f, 1.0f, 0.0f), _texcoord, _texcoord),
            new PosNormalTexcoordVertex(1.0f, 0.0f, 1.0f, GenUtils.PackF4u(0.0f, 1.0f, 0.0f), _texcoord, 0.0f),
            new PosNormalTexcoordVertex(-1.0f, 0.0f, -1.0f, GenUtils.PackF4u(0.0f, 1.0f, 0.0f), 0.0f, _texcoord),
            new PosNormalTexcoordVertex(1.0f, 0.0f, -1.0f, GenUtils.PackF4u(0.0f, 1.0f, 0.0f), 0.0f, 0.0f),
        };

        static PosNormalTexcoordVertex[] _VPlaneVertices =
        {
            new PosNormalTexcoordVertex(-1.0f, 1.0f, 0.0f, GenUtils.PackF4u(0.0f, 0.0f, -1.0f), 1.0f, 1.0f),
            new PosNormalTexcoordVertex(1.0f, 1.0f, 0.0f, GenUtils.PackF4u(0.0f, 0.0f, -1.0f), 1.0f, 0.0f),
            new PosNormalTexcoordVertex(-1.0f, -1.0f, 0.0f, GenUtils.PackF4u(0.0f, 0.0f, -1.0f), 0.0f, 1.0f),
            new PosNormalTexcoordVertex(1.0f, -1.0f, 0.0f, GenUtils.PackF4u(0.0f, 0.0f, -1.0f), 0.0f, 0.0f),
        };

        static ushort[] _planeIndices =
        {
            0, 1, 2,
            1, 3, 2,
        };

        static bool _flipV = false;
        static float _texelHalf = 0.0f;

        private SmUniformData _smUniformDataRef;

        private Material _defaultMaterial;
        private Light _pointLight;
        private Light _directionalLight;

        private Uniform _texColor;
        private readonly FrameBuffer[] _rtShadowMap = new FrameBuffer[(int) SmRenderTargets.Count];
        private FrameBuffer _rtBlur;


        // ----------------------------
        // ----------------------------
        // ----------------------------


        public SceneRenderer()
        {
            _inst = this;
            Main = new RenderTarget {FrameBuffer = new FrameBuffer(100, 100, TextureFormat.RGBA8)};
            _shadowMapManager = new ShadowMapManager();

            ClearValues = new ClearValues(0x00000000, 1.0f, 0);

            Application.Instance.Window.Resized += (sender, args) => OnWindowResize(args.Width, args.Height);

            // Mesh
            _meshProgram = ResourceLoader.LoadProgram("mesh", "mesh");

            // Skybox
            _skyboxProgram = ResourceLoader.LoadProgram("hdr_skybox", "hdr_skybox");
            _skyboxTexture = ResourceLoader.LoadTexture("uffizi.ktx");

            _textureCubeUniform = new Uniform("s_texCube", UniformType.Sampler);
            _skyboxTransform = new Uniform("u_transform", UniformType.Matrix4x4);

            // Samplers
            _textureSampler = new Uniform("s_texColor", UniformType.Sampler);
            _shadowMapSamplers[0] = new Uniform("s_shadowMap0", UniformType.Sampler);
            _shadowMapSamplers[1] = new Uniform("s_shadowMap1", UniformType.Sampler);
            _shadowMapSamplers[2] = new Uniform("s_shadowMap2", UniformType.Sampler);
            _shadowMapSamplers[3] = new Uniform("s_shadowMap3", UniformType.Sampler);

            // ----------------------------
            // TEMPORARY MESHES AND TEXURES
            // ----------------------------
            // Textures
            _texFigure = ResourceLoader.LoadTexture("figure-rgba.dds");
            _texFlare = ResourceLoader.LoadTexture("flare.dds");
            _texFieldstone = ResourceLoader.LoadTexture("fieldstone-rgba.dds");

            // Meshes
            _bunnyMesh = Mesh.Create("bunny");
            _treeMesh = Mesh.Create("bunny");
            _cubeMesh = Mesh.Create("bunny");
            _hollowcubeMesh = Mesh.Create("bunny");
            _hplaneMesh = new Mesh(MemoryBlock.FromArray(_HPlaneVertices), PosNormalTexcoordVertex.Layout,
                _planeIndices);
            _vplaneMesh = new Mesh(MemoryBlock.FromArray(_VPlaneVertices), PosNormalTexcoordVertex.Layout,
                _planeIndices);

            // ----------------------------
            // ----------------------------
            // ----------------------------


            _defaultMaterial = new Material
            {
                Ambient = new Vector4(1.0f, 1.0f, 1.0f, 0.0f),
                Diffuse = new Vector4(1.0f, 1.0f, 1.0f, 0.0f),
                Specular = new Vector4(1.0f, 1.0f, 1.0f, 0.0f)
            };

            _pointLight = new Light
            {
                Position = new Vector4(0.0f, 0.0f, 0.0f, 1.0f),
                Ambient = new Light.RgbPower(1.0f, 1.0f, 1.0f, 0.0f),
                Diffuse = new Light.RgbPower(1.0f, 1.0f, 1.0f, 850.0f),
                Specular = new Light.RgbPower(1.0f, 1.0f, 1.0f, 0.0f),
                SpotDirectionInner = new Light.SpotInner(0.0f, -0.4f, -0.6f, 0.0f),
                AttenuationSpotOuter = new Light.SpotOuter(1.0f, 0.0f, 1.0f, 91.0f)
            };

            _directionalLight = new Light
            {
                Position = new Vector4(0.5f, -1.0f, 0.1f, 0.0f),
                Ambient = new Light.RgbPower(1.0f, 1.0f, 1.0f, 0.02f),
                Diffuse = new Light.RgbPower(1.0f, 1.0f, 1.0f, 0.4f),
                Specular = new Light.RgbPower(1.0f, 1.0f, 1.0f, 0.0f),
                SpotDirectionInner = new Light.SpotInner(0.0f, 0.0f, 0.0f, 1.0f),
                AttenuationSpotOuter = new Light.SpotOuter(0.0f, 0.0f, 0.0f, 1.0f)
            };


            _smUniformDataRef = _shadowMapManager.Uniforms.Data;
            _smUniformDataRef.Material = _defaultMaterial;
            _smUniformDataRef.Light = _pointLight;
            _smUniformDataRef.Color = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            _smUniformDataRef.LightTransform = Matrix4x4.Identity;
            _smUniformDataRef.SmTransforms[0] = Matrix4x4.Identity;
            _smUniformDataRef.SmTransforms[1] = Matrix4x4.Identity;
            _smUniformDataRef.SmTransforms[2] = Matrix4x4.Identity;
            _smUniformDataRef.SmTransforms[3] = Matrix4x4.Identity;
            _shadowMapManager.Uniforms.SubmitConst();


            _shadowMapManager.Settings[(int) LightType.Spot, (int) SmDepthImpl.InvZ, (int) SmImpl.VSM] =
                new SmSettings
                {
                    Bias = 0.045f,
                    Blur = true,
                    CustomParam0 = 0.02f,
                    CustomParam1 = 450.0f,
                    DepthValuePow = 10.0f,
                    DrawProgram =
                        _shadowMapManager.Programs.ColorLighting[(int) SmType.Single, (int) SmDepthImpl.InvZ,
                            (int) SmImpl.VSM],
                    Far = 250.0f,
                    Near = 8.0f,
                    NormalOffset = 0.001f,
                    PackProgram =
                        _shadowMapManager.Programs.PackDepth[(int) SmDepthImpl.InvZ, (int) SmPackDepth.VSM],
                    XNum = 2.0f,
                    YNum = 2.0f,
                    SizePwrTwo = 11.0f,
                    XOffset = 1.0f,
                    YOffset = 1.0f
                };

            _currentSmSettings =
                _shadowMapManager.Settings[(int) LightType.Spot, (int) SmDepthImpl.InvZ, (int) SmImpl.VSM];


            // Render targets.
            var shadowMapSize = 1 << ((int) _currentSmSettings.SizePwrTwo);
            _shadowMapSize = (ushort) shadowMapSize;
            float fShadowMapSize = _shadowMapSize;


            _shadowMapManager.Uniforms.CustomParams2.SmTexelSize = 1.0f / fShadowMapSize;
            for (var i = 0; i < (int) SmRenderTargets.Count; i++)
            {
                var fbTextures = new[]
                {
                    Texture.Create2D(_shadowMapSize, _shadowMapSize, false,
                        1, TextureFormat.RGBA8, TextureFlags.RenderTarget),
                    Texture.Create2D(_shadowMapSize, _shadowMapSize, false,
                        1, TextureFormat.D24S8, TextureFlags.RenderTarget),
                };
                _rtShadowMap[i] = new FrameBuffer(fbTextures, true);
            }

            _rtBlur = new FrameBuffer(_shadowMapSize, _shadowMapSize, TextureFormat.BGRA8);
        }

        public void Begin(Scene scene)
        {
            _activeScene = scene;

            var settings = _activeScene.Settings;
        }

        public void End()
        {
            ImGui.Checkbox("Blur", ref _currentSmSettings.Blur);
            ImGui.Checkbox("Update scene", ref _activeScene.Settings.UpdateScene);
            ImGui.Checkbox("Update lights", ref _activeScene.Settings.UpdateLights);


            // Update uniforms.
            var uniforms = _shadowMapManager.Uniforms;

            uniforms.CustomParams1.ShadowMapBias = _currentSmSettings.Bias;
            uniforms.CustomParams1.ShadowMapOffset = _currentSmSettings.NormalOffset;
            uniforms.CustomParams1.ShadowMapParam0 = _currentSmSettings.CustomParam0;
            uniforms.CustomParams1.ShadowMapParam1 = _currentSmSettings.CustomParam1;

            uniforms.CustomParams2.DepthValuePow = _currentSmSettings.DepthValuePow;
            uniforms.CustomParams2.ShowSmCoverage = _activeScene.Settings.ShowSmCoverage ? 1.0f : 0.0f;

            uniforms.CustomParamsBlur.XNum = _currentSmSettings.XNum;
            uniforms.CustomParamsBlur.YNum = _currentSmSettings.YNum;
            uniforms.CustomParamsBlur.XOffset = _currentSmSettings.XOffset;
            uniforms.CustomParamsBlur.YOffset = _currentSmSettings.YOffset;

            uniforms.Data.Light = (LightType.Directional == _activeScene.Settings.LightType)
                ? _directionalLight
                : _pointLight;


            if (LightType.Spot == _activeScene.Settings.LightType)
            {
                _pointLight.AttenuationSpotOuter.Outer = _activeScene.Settings.SpotOuterAngle;
                _pointLight.SpotDirectionInner.Inner = _activeScene.Settings.SpotInnerAngle;
            }
            else
            {
                _pointLight.AttenuationSpotOuter.Outer = 91.0f; //above 90.0f means point light
            }

            _shadowMapManager.Uniforms.SubmitPerFrame();


            ////////////////////////////////////////////
            // TODO: Should be moved to Scene.OnUpdate()
            ////////////////////////////////////////////


            _pointLight.ComputeSpaceViewComponents(Camera.View);
            _directionalLight.ComputeSpaceViewComponents(Camera.View);

            var dt = Global.Clock.Frame;

            if (_activeScene.Settings.UpdateLights)
            {
                lightsTotalTime += dt.AsSeconds();
            }

            if (_activeScene.Settings.UpdateScene)
            {
                sceneTotalTime += dt.AsSeconds();
            }

            _pointLight.Position.X = (float) Math.Cos(lightsTotalTime) * 20.0f;
            _pointLight.Position.Y = 26.0f;
            _pointLight.Position.Z = (float) Math.Sin(lightsTotalTime) * 20.0f;

            _pointLight.SpotDirectionInner.X = -_pointLight.Position.X;
            _pointLight.SpotDirectionInner.Y = -_pointLight.Position.Y;
            _pointLight.SpotDirectionInner.Z = -_pointLight.Position.Z;

            _directionalLight.Position.X = -(float) Math.Cos(lightsTotalTime);
            _directionalLight.Position.Y = -1.0f;
            _directionalLight.Position.Z = -(float) Math.Sin(lightsTotalTime);


            var floorTransform = Matrix4x4.CreateScale(550.0f);
            var bunnyTransform = Matrix4x4.CreateScale(5.0f) *
                                 Matrix4x4.CreateRotationY(1.56f - sceneTotalTime / 2.0f) *
                                 Matrix4x4.CreateTranslation(15.0f, -1.0f, 0.0f);
            var hollowCubeTransform = Matrix4x4.CreateScale(2.5f) *
                                      Matrix4x4.CreateRotationY(1.56f - sceneTotalTime * 2.0f) *
                                      Matrix4x4.CreateTranslation(0.0f, 10.0f, 0.0f);
            var cubeTransform = Matrix4x4.CreateScale(2.5f) *
                                Matrix4x4.CreateRotationY(1.56f - sceneTotalTime) *
                                Matrix4x4.CreateTranslation(-15.0f, 5.0f, 0.0f);

            const int noTrees = 10;
            var treesTransform = new Matrix4x4[noTrees];
            for (var i = 0; i < noTrees; i++)
            {
                treesTransform[i] = Matrix4x4.CreateScale(5.0f) * Matrix4x4.CreateRotationY(i) *
                                    Matrix4x4.CreateTranslation(
                                        (float) (Math.Sin(i * 2.0f * Math.PI / noTrees) * 10.0f),
                                        0.0f,
                                        (float) (Math.Cos(i * 2.0f * Math.PI / noTrees) * 10.0f)
                                    );
            }

            // Compute transform matrices.
            const int noShadowMapPasses = (int) SmRenderTargets.Count;
            var lightView = new Matrix4x4[noShadowMapPasses];
            var lightProj = new Matrix4x4[noShadowMapPasses];
            var mtxYpr = new Matrix4x4[(int) SmDebugTetrahedronFaces.Count];

            var screenProj = Matrix4x4.CreateOrthographicOffCenter(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
            var screenView = Matrix4x4.Identity;


            // Update render target size.
            var shadowMapSize = 1 << (int) _currentSmSettings.SizePwrTwo;
            _shadowMapSize = shadowMapSize;
            // if ( bLtChanged || m_currentShadowMapSize != shadowMapSize )
            // {
            //     m_currentShadowMapSize = shadowMapSize;
            //     s_uniforms.m_shadowMapTexelSize = 1.0f / currentShadowMapSizef;
            //
            //     {
            //         Bgfx.destroy(s_rtShadowMap[0]);
            //
            //         Bgfx.TextureHandle fbtextures[] =
            //         {
            //             Bgfx.createTexture2D(m_currentShadowMapSize, m_currentShadowMapSize, false, 1, Bgfx.TextureFormat.BGRA8, BGFX_TEXTURE_RT),
            //             Bgfx.createTexture2D(m_currentShadowMapSize, m_currentShadowMapSize, false, 1, Bgfx.TextureFormat.D24S8, BGFX_TEXTURE_RT),
            //         };
            //         s_rtShadowMap[0] = Bgfx.createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures, true);
            //     }
            //
            //     if ( LightType.DirectionalLight == settings.m_lightType )
            //     {
            //         for ( uint8_t ii = 1; ii < ShadowMapRenderTargets.Count; ++ii )
            //         {
            //             {
            //                 Bgfx.destroy(s_rtShadowMap[ii]);
            //
            //                 Bgfx.TextureHandle fbtextures[] =
            //                 {
            //                     Bgfx.createTexture2D(m_currentShadowMapSize, m_currentShadowMapSize, false, 1, Bgfx.TextureFormat.BGRA8, BGFX_TEXTURE_RT),
            //                     Bgfx.createTexture2D(m_currentShadowMapSize, m_currentShadowMapSize, false, 1, Bgfx.TextureFormat.D24S8, BGFX_TEXTURE_RT),
            //                 };
            //                 s_rtShadowMap[ii] = Bgfx.createFrameBuffer(BX_COUNTOF(fbtextures), fbtextures, true);
            //             }
            //         }
            //     }
            //
            //     Bgfx.destroy(s_rtBlur);
            //     s_rtBlur = Bgfx.createFrameBuffer(m_currentShadowMapSize, m_currentShadowMapSize, Bgfx.TextureFormat.BGRA8);
            // }

            var settings = _activeScene.Settings;

            if (settings.LightType == LightType.Spot)
            {
                var fovY = GenUtils.ToRadians(settings.CoverageSpotL);
                const float aspect = 1.0f;
                lightProj[(int) LightProjType.Horizontal] = Matrix4x4.CreatePerspectiveFieldOfView(
                    fovY,
                    aspect,
                    _currentSmSettings.Near,
                    _currentSmSettings.Far
                );

                var pointPosVec3 = new Vector3(_pointLight.Position.X, _pointLight.Position.Y, _pointLight.Position.Z);
                var pointDirVec3 = new Vector3(_pointLight.SpotDirectionInner.X, _pointLight.SpotDirectionInner.Y,
                    _pointLight.SpotDirectionInner.Z);

                // var at = pointPosVec3 + pointDirVec3;
                lightView[(int) SmDebugTetrahedronFaces.Green] =
                    Matrix4x4.CreateLookAt(pointPosVec3, pointDirVec3, new Vector3(0, 1, 0));
            }
            // else if

            // Reset render targets.
            var invalidFb = FrameBuffer.Invalid;
            for (ushort i = 0; i <= (ushort) (ViewID.DrawDepth3); i++)
            {
                Bgfx.SetViewFrameBuffer(i, invalidFb);
                Bgfx.SetViewRect(i, 0, 0, (int) Main.Width, (int) Main.Height);
            }

            // Determine on-screen rectangle size where depth buffer will be drawn.
            var depthRectHeight = (int) (Main.Height / 2.5f);
            var depthRectWidth = depthRectHeight;
            var depthRectX = 0;
            var depthRectY = (int) Main.Height - depthRectHeight;

            // Setup views and render targets.
            Bgfx.SetViewRect(0, 0, 0, (int) Main.Width, (int) Main.Height);
            unsafe
            {
                var view = Camera.View;
                var proj = Camera.Projection;

                Bgfx.SetViewTransform(0, (float*) &view, (float*) &proj);
            }

            if (_activeScene.Settings.LightType == LightType.Spot)
            {
                // View.ShadowMap0 - Clear shadow map. (used as convenience, otherwise render_pass_1 could be cleared)
                // View.ShadowMap1 - Craft shadow map.
                // View.VBlur0     - Vertical blur.
                // View.HBlur0     - Horizontal blur.
                // View.DrawScene0 - Draw scene.
                // View.DrawScene1 - Draw floor bottom.
                // View.DrawScene0 - Draw depth buffer.

                // Bgfx.SetViewRect((ushort) ViewID.BackBuffer, 0, 0, (int) Main.Width, (int) Main.Height);
                Bgfx.SetViewRect((ushort) ViewID.ShadowMap0, 0, 0, _shadowMapSize, _shadowMapSize);
                Bgfx.SetViewRect((ushort) ViewID.ShadowMap1, 0, 0, _shadowMapSize, _shadowMapSize);
                Bgfx.SetViewRect((ushort) ViewID.VBlur0, 0, 0, _shadowMapSize, _shadowMapSize);
                Bgfx.SetViewRect((ushort) ViewID.HBlur0, 0, 0, _shadowMapSize, _shadowMapSize);
                Bgfx.SetViewRect((ushort) ViewID.DrawScene0, 0, 0, (int) Main.Width, (int) Main.Height);
                Bgfx.SetViewRect((ushort) ViewID.DrawScene1, 0, 0, (int) Main.Width, (int) Main.Height);
                Bgfx.SetViewRect((ushort) ViewID.DrawDepth0, depthRectX, depthRectY, depthRectWidth, depthRectHeight);

                unsafe
                {
                    var lightView0 = lightView[0];
                    var lightProjHorizontal = lightProj[(int) LightProjType.Horizontal];
                    var view = Camera.View;
                    var proj = Camera.Projection;

                    // Bgfx.SetViewTransform((ushort) ViewID.BackBuffer, (float*) &view, (float*) &proj);k
                    Bgfx.SetViewTransform((ushort) ViewID.ShadowMap0, (float*) &screenView, (float*) &screenProj);
                    Bgfx.SetViewTransform((ushort) ViewID.ShadowMap1, (float*) &lightView0,
                        (float*) &lightProjHorizontal);
                    Bgfx.SetViewTransform((ushort) ViewID.VBlur0, (float*) &screenView, (float*) &screenProj);
                    Bgfx.SetViewTransform((ushort) ViewID.HBlur0, (float*) &screenView, (float*) &screenProj);
                    Bgfx.SetViewTransform((ushort) ViewID.DrawScene0, (float*) &view, (float*) &proj);
                    Bgfx.SetViewTransform((ushort) ViewID.DrawScene1, (float*) &view, (float*) &proj);
                    Bgfx.SetViewTransform((ushort) ViewID.DrawDepth0, (float*) &screenView, (float*) &screenProj);
                }

                Bgfx.SetViewFrameBuffer((ushort) ViewID.BackBuffer, Main.FrameBuffer);
                Bgfx.SetViewFrameBuffer((ushort) ViewID.DrawScene0, Main.FrameBuffer);
                Bgfx.SetViewFrameBuffer((ushort) ViewID.DrawScene1, Main.FrameBuffer);
                Bgfx.SetViewFrameBuffer((ushort) ViewID.ShadowMap0, _rtShadowMap[0]);
                Bgfx.SetViewFrameBuffer((ushort) ViewID.ShadowMap1, _rtShadowMap[0]);
                Bgfx.SetViewFrameBuffer((ushort) ViewID.VBlur0, _rtBlur);
                Bgfx.SetViewFrameBuffer((ushort) ViewID.HBlur0, _rtShadowMap[0]);
            }
            // else if (LightType.PointLight == settings.m_lightType)

            // Clear backbuffer at beginning.
            Bgfx.SetViewClear(0,
                ClearTargets.Color | ClearTargets.Depth,
                ClearValues.Rgba,
                ClearValues.Depth,
                ClearValues.Stencil
            );
            Bgfx.Touch(0);

            // Clear shadowmap rendertarget at beginning.
            var flags0 = (_activeScene.Settings.LightType == LightType.Directional)
                    ? 0
                    : ClearTargets.Color | ClearTargets.Depth | ClearTargets.Stencil
                ;

            Bgfx.SetViewClear((ushort) ViewID.ShadowMap0, flags0, 0xfefefefe, ClearValues.Depth, ClearValues.Stencil);
            Bgfx.Touch((ushort) ViewID.ShadowMap0);


            var flags1 = (_activeScene.Settings.LightType == LightType.Directional)
                    ? 0
                    : ClearTargets.Color | ClearTargets.Depth
                ;

            for (var i = 0; i < 4; i++)
            {
                Bgfx.SetViewClear(
                    (ushort) ((int) ViewID.ShadowMap1 + i),
                    flags1,
                    0xfefefefe,
                    ClearValues.Depth,
                    ClearValues.Stencil
                );
                Bgfx.Touch((ushort) ((int) ViewID.ShadowMap1 + i));
            }


            // Render.

            // Craft shadow map.
            {
                // Craft stencil mask for point light shadow map packing.

                if (settings.LightType == LightType.Point && settings.StencilPack)
                {
                    // if ( 6 == Bgfx.getAvailTransientVertexBuffer(6, m_posLayout) )
                    // {
                    // 	struct Pos
                    // 	{
                    // 		float m_x, m_y, m_z;
                    // 	};
                    //
                    // 	Bgfx.TransientVertexBuffer vb;
                    // 	Bgfx.allocTransientVertexBuffer(&vb, 6, m_posLayout);
                    // 	Pos *vertex = (Pos *)vb.data;
                    //
                    // 	const float min = 0.0f;
                    // 	const float max = 1.0f;
                    // 	const float center = 0.5f;
                    // 	const float zz = 0.0f;
                    //
                    // 	vertex[0].m_x = min;
                    // 	vertex[0].m_y = min;
                    // 	vertex[0].m_z = zz;
                    //
                    // 	vertex[1].m_x = max;
                    // 	vertex[1].m_y = min;
                    // 	vertex[1].m_z = zz;
                    //
                    // 	vertex[2].m_x = center;
                    // 	vertex[2].m_y = center;
                    // 	vertex[2].m_z = zz;
                    //
                    // 	vertex[3].m_x = center;
                    // 	vertex[3].m_y = center;
                    // 	vertex[3].m_z = zz;
                    //
                    // 	vertex[4].m_x = max;
                    // 	vertex[4].m_y = max;
                    // 	vertex[4].m_z = zz;
                    //
                    // 	vertex[5].m_x = min;
                    // 	vertex[5].m_y = max;
                    // 	vertex[5].m_z = zz;
                    //
                    // 	Bgfx.SetState(0);
                    // 	Bgfx.SetStencil(BGFX_STENCIL_TEST_ALWAYS
                    // 					 | BGFX_STENCIL_FUNC_REF(1)
                    // 					 | BGFX_STENCIL_FUNC_RMASK(0xff)
                    // 					 | BGFX_STENCIL_OP_FAIL_S_REPLACE
                    // 					 | BGFX_STENCIL_OP_FAIL_Z_REPLACE
                    // 					 | BGFX_STENCIL_OP_PASS_Z_REPLACE
                    // 	);
                    // 	Bgfx.SetVertexBuffer(0, &vb);
                    // 	Bgfx.Submit(RENDERVIEW_SHADOWMAP_0_ID, s_programs.m_black);
                    // }
                }

                // Draw scene into shadowmap.
                var drawNum = settings.LightType switch
                {
                    LightType.Spot => 1,
                    LightType.Point => 4,
                    LightType.Directional => settings.NoSplits,
                    _ => 0
                };

                for (var i = 0; i < drawNum; i++)
                {
                    var viewId = (ushort) ((int) ViewID.ShadowMap1 + i);

                    const int renderStateIndex = (int) SmRenderStateGroupType.SmPackDepth;
                    if (settings.LightType == LightType.Point && settings.StencilPack)
                    {
                        // renderStateIndex = uint8_t((ii < 2)
                        //     ? RenderState.ShadowMap_PackDepthHoriz
                        //     : RenderState.ShadowMap_PackDepthVert);
                    }

                    // Floor.
                    _hplaneMesh.Submit(
                        viewId,
                        _currentSmSettings.PackProgram,
                        floorTransform,
                        ShadowMapManager.StateGroups[renderStateIndex],
                        false,
                        _shadowMapManager.Uniforms
                    );

                    // Bunny.
                    _bunnyMesh.Submit(
                        viewId,
                        _currentSmSettings.PackProgram,
                        bunnyTransform,
                        ShadowMapManager.StateGroups[renderStateIndex],
                        false,
                        _shadowMapManager.Uniforms
                    );

                    // Hollow cube.
                    _hollowcubeMesh.Submit(
                        viewId,
                        _currentSmSettings.PackProgram,
                        hollowCubeTransform,
                        ShadowMapManager.StateGroups[renderStateIndex],
                        false,
                        _shadowMapManager.Uniforms
                    );

                    // Cube.
                    _cubeMesh.Submit(
                        viewId,
                        _currentSmSettings.PackProgram,
                        cubeTransform,
                        ShadowMapManager.StateGroups[renderStateIndex],
                        false,
                        _shadowMapManager.Uniforms
                    );

                    // Trees.
                    for (var j = 0; j < noTrees; j++)
                    {
                        _treeMesh.Submit(
                            viewId,
                            _currentSmSettings.PackProgram,
                            treesTransform[j],
                            ShadowMapManager.StateGroups[renderStateIndex],
                            false,
                            _shadowMapManager.Uniforms
                        );
                    }
                }
            }

            var depthType = SmPackDepth.VSM;


            // Blur shadow map.
            if (_currentSmSettings.Blur)
            {
                Bgfx.SetTexture(4, _shadowMapSamplers[0], _rtShadowMap[0].GetTexture());
                Bgfx.SetRenderState(RenderState.WriteRGB | RenderState.WriteA);

                BindScreenSpaceQuad(shadowMapSize, shadowMapSize, _flipV);

                Bgfx.Submit((ushort) ViewID.VBlur0, _shadowMapManager.Programs.VBlur[(int) depthType]);

                Bgfx.SetTexture(4, _shadowMapSamplers[0], _rtBlur.GetTexture());
                Bgfx.SetRenderState(RenderState.WriteRGB | RenderState.WriteA);

                BindScreenSpaceQuad(shadowMapSize, shadowMapSize, _flipV);

                Bgfx.Submit((ushort) ViewID.HBlur0, _shadowMapManager.Programs.HBlur[(int) depthType]);

                if (settings.LightType == LightType.Directional)
                {
                    for (int i = 1, j = 2; i < settings.NoSplits; i++, j += 2)
                    {
                        var viewId = (ushort) ((int) ViewID.VBlur0 + j);

                        Bgfx.SetTexture(4, _shadowMapSamplers[0], _rtShadowMap[i].GetTexture());
                        Bgfx.SetRenderState(RenderState.WriteRGB | RenderState.WriteA);

                        BindScreenSpaceQuad(shadowMapSize, shadowMapSize, _flipV);

                        Bgfx.Submit(viewId, _shadowMapManager.Programs.VBlur[(int) depthType]);

                        Bgfx.SetTexture(4, _shadowMapSamplers[0], _rtBlur.GetTexture());
                        Bgfx.SetRenderState(RenderState.WriteRGB | RenderState.WriteA);

                        BindScreenSpaceQuad(shadowMapSize, shadowMapSize, _flipV);

                        Bgfx.Submit((ushort) (viewId + 1), _shadowMapManager.Programs.HBlur[(int) depthType]);
                    }
                }
            }


            // Draw scene.
            {
                // Setup shadow mtx.
                var shadowTransform = Matrix4x4.Identity;

                var ymul = _flipV ? 0.5f : -0.5f;
                var zadd = 0.5f;

                var biasTransform = new Matrix4x4(
                    0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, ymul, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.5f, 0.0f,
                    0.5f, 0.5f, zadd, 1.0f
                );

                switch (settings.LightType)
                {
                    case LightType.Spot:
                    {
                        var tmp = lightProj[(int) LightProjType.Horizontal] * biasTransform;
                        shadowTransform = lightView[0] * tmp;
                        break;
                    }
                    case LightType.Point:
                    {
                        var s = _flipV ? 1.0f : -1.0f; //sign
                        zadd = 0.5f;

                        var cropBiasTransforms = new Matrix4x4 [2, (int) SmDebugTetrahedronFaces.Count]
                        {
                            {
                                // settings.m_stencilPack == false

                                new Matrix4x4(
                                    // D3D: Green, OGL: Blue
                                    0.25f, 0.0f, 0.0f, 0.0f,
                                    0.0f, s * 0.25f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.25f, 0.25f, zadd, 1.0f
                                ),
                                new Matrix4x4(
                                    // D3D: Yellow, OGL: Red
                                    0.25f, 0.0f, 0.0f, 0.0f,
                                    0.0f, s * 0.25f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.75f, 0.25f, zadd, 1.0f
                                ),
                                new Matrix4x4(
                                    // D3D: Blue, OGL: Green
                                    0.25f, 0.0f, 0.0f, 0.0f,
                                    0.0f, s * 0.25f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.25f, 0.75f, zadd, 1.0f
                                ),
                                new Matrix4x4(
                                    // D3D: Red, OGL: Yellow
                                    0.25f, 0.0f, 0.0f, 0.0f,
                                    0.0f, s * 0.25f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.75f, 0.75f, zadd, 1.0f
                                ),
                            },
                            {
                                // settings.m_stencilPack == true
                                new Matrix4x4(
                                    // D3D: Red, OGL: Blue
                                    0.25f, 0.0f, 0.0f, 0.0f,
                                    0.0f, s * 0.5f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.25f, 0.5f, zadd, 1.0f
                                ),
                                new Matrix4x4(
                                    // D3D: Blue, OGL: Red
                                    0.25f, 0.0f, 0.0f, 0.0f,
                                    0.0f, s * 0.5f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.75f, 0.5f, zadd, 1.0f
                                ),
                                new Matrix4x4(
                                    // D3D: Green, OGL: Green
                                    0.5f, 0.0f, 0.0f, 0.0f,
                                    0.0f, s * 0.25f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.5f, 0.75f, zadd, 1.0f
                                ),
                                new Matrix4x4(
                                    // D3D: Yellow, OGL: Yellow
                                    0.5f, 0.0f, 0.0f, 0.0f,
                                    0.0f, s * 0.25f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.5f, 0.25f, zadd, 1.0f
                                ),
                            }
                        };


                        //Use as: [stencilPack][flipV][tetrahedronFace]
                        var cropBiasIndices = new byte[2, 2, 4]
                        {
                            {{0, 1, 2, 3}, {2, 3, 0, 1}},
                            {{3, 2, 0, 1}, {2, 3, 0, 1},}
                        };


                        for (var i = 0; i < (int) SmDebugTetrahedronFaces.Count; ++i)
                        {
                            var projType = settings.StencilPack
                                ? (i > 1 ? LightProjType.Horizontal : LightProjType.Vertical)
                                : LightProjType.Horizontal;

                            var biasIndex = cropBiasIndices[settings.StencilPack ? 1 : 0, _flipV ? 1 : 0, i];

                            var tmp = mtxYpr[i] * lightProj[(int) projType];
                            _smUniformDataRef.SmTransforms[i] =
                                tmp * cropBiasTransforms[settings.StencilPack ? 1 : 0, biasIndex];
                        }

                        var pointPos = _pointLight.Position;
                        shadowTransform *= Matrix4x4.CreateTranslation(-pointPos.X, -pointPos.Y, -pointPos.Z);
                        break;
                    }
                    case LightType.Directional:
                    {
                        for (var i = 0; i < settings.NoSplits; ++i)
                        {
                            var tmp = lightProj[i] * biasTransform;
                            _smUniformDataRef.SmTransforms[i] = lightView[0] * tmp;
                        }

                        break;
                    }
                    default:
                        throw new ArgumentOutOfRangeException();
                }

                // Floor.
                if (settings.LightType != LightType.Directional)
                {
                    _smUniformDataRef.LightTransform = floorTransform * shadowTransform;
                }


                _hplaneMesh.Submit(
                    (ushort) ViewID.DrawScene0,
                    _currentSmSettings.DrawProgram,
                    floorTransform,
                    ShadowMapManager.StateGroups[(int) SmRenderStateGroupType.Default],
                    true, _shadowMapManager.Uniforms
                );

                // Bunny.
                if (settings.LightType != LightType.Directional)
                {
                    _smUniformDataRef.LightTransform = bunnyTransform * shadowTransform;
                }

                _bunnyMesh.Submit(
                    (ushort) ViewID.DrawScene0,
                    _currentSmSettings.DrawProgram,
                    bunnyTransform,
                    ShadowMapManager.StateGroups[(int) SmRenderStateGroupType.Default],
                    true, _shadowMapManager.Uniforms
                );

                // Hollow cube.
                if (settings.LightType != LightType.Directional)
                {
                    _smUniformDataRef.LightTransform = hollowCubeTransform * shadowTransform;
                }

                _hollowcubeMesh.Submit(
                    (ushort) ViewID.DrawScene0,
                    _currentSmSettings.DrawProgram,
                    hollowCubeTransform,
                    ShadowMapManager.StateGroups[(int) SmRenderStateGroupType.Default],
                    true, _shadowMapManager.Uniforms
                );

                // Cube.
                if (settings.LightType != LightType.Directional)
                {
                    _smUniformDataRef.LightTransform = cubeTransform * shadowTransform;
                }

                _cubeMesh.Submit(
                    (ushort) ViewID.DrawScene0,
                    _currentSmSettings.DrawProgram,
                    cubeTransform,
                    ShadowMapManager.StateGroups[(int) SmRenderStateGroupType.Default],
                    true, _shadowMapManager.Uniforms
                );


                // Trees.
                for (var i = 0; i < noTrees; ++i)
                {
                    if (settings.LightType != LightType.Directional)
                    {
                        _smUniformDataRef.LightTransform = treesTransform[i] * shadowTransform;
                    }

                    _treeMesh.Submit(
                        (ushort) ViewID.DrawScene0,
                        _currentSmSettings.DrawProgram,
                        treesTransform[i],
                        ShadowMapManager.StateGroups[(int) SmRenderStateGroupType.Default],
                        true, _shadowMapManager.Uniforms
                    );
                }

                static Matrix4x4 Billboard(Matrix4x4 view, Vector3 pos, Vector3 scale)
                {
                    return new Matrix4x4
                    {
                        M11 = view.M11 * scale.X,
                        M12 = view.M21 * scale.X,
                        M13 = view.M31 * scale.X,
                        M14 = 0.0f,
                        M21 = view.M12 * scale.Y,
                        M22 = view.M22 * scale.Y,
                        M23 = view.M32 * scale.Y,
                        M24 = 0.0f,
                        M31 = view.M13 * scale.Z,
                        M32 = view.M23 * scale.Z,
                        M33 = view.M33 * scale.Z,
                        M34 = 0.0f,
                        M41 = pos.X,
                        M42 = pos.Y,
                        M43 = pos.Z,
                        M44 = 1.0f
                    };
                }


                // Lights.
                if (settings.LightType == LightType.Spot || settings.LightType == LightType.Point)
                {
                    var lightScale = new Vector3(1.5f, 1.5f, 1.5f);

                    var pointPos3 = new Vector3(_pointLight.Position.X, _pointLight.Position.Y, _pointLight.Position.Z);
                    var billboardTransform = Billboard(Camera.View, pointPos3, lightScale);


                    _vplaneMesh.Submit(
                        (ushort) ViewID.DrawScene0,
                        _shadowMapManager.Programs.ColorTexture,
                        billboardTransform,
                        ShadowMapManager.StateGroups[(int) SmRenderStateGroupType.CustomBlendLightTexture],
                        false, _shadowMapManager.Uniforms, _texFlare
                    );
                }


                floorTransform *= Matrix4x4.CreateTranslation(0.0f, 0.1f, 0.0f);

                _hplaneMesh.Submit(
                    (ushort) ViewID.DrawScene1,
                    _shadowMapManager.Programs.Texture,
                    floorTransform,
                    ShadowMapManager.StateGroups[(int) SmRenderStateGroupType.CustomDrawPlaneBottom],
                    false, _shadowMapManager.Uniforms, _texFigure
                );
            }


            var handle = Gui.AddTexture(_rtShadowMap[0].GetTexture());
            ImGui.Begin("Shadow map");
            ImGui.Image(handle, Vector2.One * _shadowMapSize, Vector2.Zero, Vector2.One);
            ImGui.End();


            // Draw depth rect.
            if (settings.DrawDepthBuffer)
            {
                // Bgfx.SetTexture(4, _shadowMapSamlpers[0], _rtShadowMap[0].GetTexture());
                // Bgfx.SetRenderState(RenderState.WriteRGB | RenderState.WriteA);
                //
                // BindScreenSpaceQuad(shadowMapSize, shadowMapSize, _flipV);
                // Bgfx.Submit((ushort) ViewID.DrawDepth0, _shadowMapManager.Programs.DrawDepth[(int) depthType]);
                //
                // if (LightType.DirectionalLight == settings.m_lightType)
                // {
                //     for (uint8_t ii = 1; ii < settings.m_numSplits; ++ii)
                //     {
                //         bgfx.setTexture(4, s_shadowMap[0], bgfx.getTexture(s_rtShadowMap[ii]));
                //         bgfx.setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
                //         screenSpaceQuad(currentShadowMapSizef, currentShadowMapSizef, s_flipV);
                //         bgfx.Submit(RENDERVIEW_DRAWDEPTH_0_ID + ii, s_programs.m_drawDepth[depthType]);
                //     }
                // }
            }


            //////////////////////////////////////////////////
            // TODO: ABOVE should be moved to Scene.OnUpdate()
            //////////////////////////////////////////////////


            ExecuteSubmitions();

            _inst._scheduledRendering.Clear();
            _inst._renderCommands.Clear();

            _activeScene = null;
        }

        public static void Sumbit(Action renderAction)
        {
            _inst._scheduledRendering.Add(renderAction);
        }

        public static void Sumbit(Mesh mesh, Matrix4x4 transform)
        {
            _inst._renderCommands.Add(new RenderCommand(mesh, transform));
        }

        public static void SubmitShadowMaps()
        {
            for (var i = 0; i < (int) SmRenderTargets.Count; i++)
            {
                Bgfx.SetTexture((byte) (4 + i), _inst._shadowMapSamplers[i], _inst._rtShadowMap[i].GetTexture());
            }
        }

        private void ExecuteSubmitions()
        {
            if (Main.AwaitingResize)
            {
                var scale = Application.Instance.Window.Size;
                // Bgfx.SetViewRect((ushort) ViewID.Main, 0, 0, (int) 1280, (int) 720);
                Main.Resize();
            }


            var colorUint = (uint) ((10 << 24) |
                                    (10 << 16) |
                                    (10 << 8));

            // Bgfx.SetViewClear((ushort) ViewID.BackBuffer, ClearTargets.Color | ClearTargets.Depth, colorUint);
            Bgfx.SetViewClear((ushort) ViewID.Gui, ClearTargets.Color | ClearTargets.Depth | ClearTargets.Stencil,
                ClearValues.Rgba, ClearValues.Depth, ClearValues.Stencil);

            // Main.FrameBuffer = _rtShadowMap[1];


            // foreach (var viewID in Enum.GetValues(typeof(ViewID)))
            // {
            //     Bgfx.SetViewClear((ushort) viewID, ClearTargets.Color | ClearTargets.Depth, colorUint);
            // }

            // var viewMatrix = Matrix4x4.CreateLookAt(new Vector3(0.0f, 0.0f, -35.0f), Vector3.Zero, Vector3.UnitY);
            // var projMatrix = Matrix4x4.CreatePerspectiveFieldOfView((float) Math.PI / 3,
            //     (float) 1280 / 720, 0.1f, 100.0f);
            // unsafe
            // {
            //     Bgfx.SetViewTransform((ushort) ViewID.BackBuffer,  &viewMatrix.M11, &projMatrix.M11);
            // }

            // Bgfx.Touch(0);

            // unsafe
            // {
            //     var view = Camera.View;
            //     var projection = Camera.Projection;
            //     Bgfx.SetViewTransform((ushort) ViewID.BackBuffer, null, (float*) &projection);
            // }
            //
            //
            // Bgfx.SetTexture(0, _textureCubeUniform, _skyboxTexture);
            // Bgfx.SetRenderState(RenderState.WriteRGB |
            //                     RenderState.WriteA);
            // unsafe
            // {
            //     Matrix4x4 nonInvert = Camera.View * Camera.Projection;
            //     Matrix4x4.Invert(Camera.View * Camera.Projection, out var inverted);
            //     Bgfx.SetUniform(_skyboxTransform, (float*) &nonInvert);
            // }

            // Bgfx.SetVertexBuffer(0, _screenSpaceRectVertexBuffer);
            // Bgfx.SetIndexBuffer(_screenSpaceRectIndexBuffer);

            // Bgfx.Submit((ushort) ViewID.BackBuffer, _skyboxProgram);

            // Bgfx.SetRenderState(RenderState.Default);


            var group = new RenderStateGroup(
                RenderState.WriteRGB |
                RenderState.WriteZ |
                RenderState.WriteA |
                RenderState.DepthTestLess |
                RenderState.CullClockwise |
                RenderState.Multisampling,
                uint.MaxValue,
                StencilFlags.None,
                StencilFlags.None);


            foreach (var renderCommand in _renderCommands)
            {
                renderCommand.Mesh.Submit((byte) ViewID.DrawScene0, _meshProgram, renderCommand.Transform,
                    group);
            }

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


        private void BindScreenSpaceQuad(
            float textureWidth,
            float textureHeight,
            bool originBottomLeft = true,
            float width = 1.0f,
            float height = 1.0f)
        {
            if (TransientVertexBuffer.GetAvailableSpace(3, PosColorTexCoordVertex.Layout) != 3) return;
            
            
            Bgfx.AllocateTransientBuffers(3, PosColorTexCoordVertex.Layout, 0, out var vb, out var indexBuffer);

            var vertices = new PosColorTexCoordVertex[3];

            const float zz = 0.0f;

            var minX = -width;
            var maxX = width;
            var minY = 0.0f;
            var maxY = height * 2.0f;

            var texelHalfW = _texelHalf / textureWidth;
            var texelHalfH = _texelHalf / textureHeight;
            var minU = -1.0f + texelHalfW;
            var maxU = 1.0f + texelHalfW;

            var minV = texelHalfH;
            var maxV = 2.0f + texelHalfH;

            if (originBottomLeft)
            {
                var tmp = minV;
                minV = maxV - 1.0f;
                maxV = tmp - 1.0f;
            }

            vertices[0].X = minX;
            vertices[0].Y = minY;
            vertices[0].Z = zz;
            vertices[0].Rgba = 0xffffffff;
            vertices[0].U = minU;
            vertices[0].V = minV;

            vertices[1].X = maxX;
            vertices[1].Y = minY;
            vertices[1].Z = zz;
            vertices[1].Rgba = 0xffffffff;
            vertices[1].U = maxU;
            vertices[1].V = minV;

            vertices[2].X = maxX;
            vertices[2].Y = maxY;
            vertices[2].Z = zz;
            vertices[2].Rgba = 0xffffffff;
            vertices[2].U = maxU;
            vertices[2].V = maxV;

            var longPtr = vb.Data.ToInt64(); // Must work both on x86 and x64
            foreach (var vertex in vertices)
            {
                var currentVertexPtr = new IntPtr(longPtr);
                Marshal.StructureToPtr(vertex, currentVertexPtr, false); // You do not need to erase struct in this case
                longPtr += Marshal.SizeOf(typeof(PosColorTexCoordVertex));
            }
                
            Bgfx.SetVertexBuffer(0, vb);
        }
    }
}