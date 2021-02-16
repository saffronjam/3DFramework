using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using Renderer;
using SaffronEngine.Common;

namespace SaffronEngine.Rendering
{
    public class SmSettings
    {
        public float SizePwrTwo;
        public float DepthValuePow;
        public float Near;
        public float Far;
        public float Bias;
        public float NormalOffset;
        public float CustomParam0;
        public float CustomParam1;
        public float XNum;
        public float YNum;
        public float XOffset;
        public float YOffset;
        public bool Blur;
        public Program PackProgram;
        public Program DrawProgram;
    }

    public enum SmRenderTargets
    {
        First,
        Second,
        Third,
        Fourth,
        Count
    };

/*
     * Supporting only VSM at the moment
     */
    public enum SmImpl
    {
        VSM,
        Count
    }

    public enum SmType
    {
        Single,
        Omni,
        Cascade,
        Count
    }

    /*
     * Supporting only VSM at the moment
     */
    public enum SmPackDepth
    {
        VSM,
        Count
    };

    /*
     * Supporting only InvZ at the moment
     */
    public enum SmDepthImpl
    {
        InvZ,
        Count
    };

    public class SmUniformData
    {
        public Matrix4x4 LightTransform;
        public Vector4 Color;
        public Light Light;
        public Matrix4x4[] SmTransforms = new Matrix4x4[4];
        public Material Material;
    }

    public enum SmDebugTetrahedronFaces
    {
        Green,
        Yellow,
        Blue,
        Red,
        Count
    };

    public class SmUniforms : IUniformGroup
    {
        private readonly float[] _csmFarDistances = new float[4]
        {
            30.0f,
            90.0f,
            180.0f,
            1000.0f
        };

        private readonly float[] _tetraNormalGreen = new float[3]
        {
            0.0f,
            -0.57735026f,
            0.81649661f
        };

        private readonly float[] _tetraNormalYellow = new float[3]
        {
            0.0f,
            -0.57735026f,
            -0.81649661f
        };

        private readonly float[] _tetraNormalBlue = new float[3]
        {
            -0.81649661f,
            0.57735026f,
            0.0f
        };

        private readonly float[] _tetraNormalRed = new float[3]
        {
            0.81649661f,
            0.57735026f,
            0.0f,
        };

        private float _XNum = 2.0f;
        private float _YNum = 2.0f;
        private float _XOffset = 10.0f / 512.0f;
        private float _YOffset = 10.0f / 512.0f;

        private Uniform _uParams0;
        private Uniform _uParams1;
        private Uniform _uParams2;
        private Uniform _uColor;
        private Uniform _uSmSamplingParams;
        private Uniform _uCsmFarDistances;
        private Uniform _uLightMtx;

        private Uniform _uTetraNormalGreen;
        private Uniform _uTetraNormalYellow;
        private Uniform _uTetraNormalBlue;
        private Uniform _uTetraNormalRed;

        private Uniform _uShadowMapMtx0;
        private Uniform _uShadowMapMtx1;
        private Uniform _uShadowMapMtx2;
        private Uniform _uShadowMapMtx3;

        private Uniform _uLightPosition;
        private Uniform _uLightAmbientPower;
        private Uniform _uLightDiffusePower;
        private Uniform _uLightSpecularPower;
        private Uniform _uLightSpotDirectionInner;
        private Uniform _uLightAttenuationSpotOuter;

        private Uniform _uMaterialKa;
        private Uniform _uMaterialKd;
        private Uniform _uMaterialKs;

        [StructLayout(LayoutKind.Sequential)]
        public struct Params0
        {
            public float AmbientPass;
            public float LightingPass;
            public float Unused00;
            public float Unused01;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Params1
        {
            public float ShadowMapBias;
            public float ShadowMapOffset;
            public float ShadowMapParam0;
            public float ShadowMapParam1;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Params2
        {
            public float DepthValuePow;
            public float ShowSmCoverage;
            public float SmTexelSize;
            public float Unused23;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ParamsBlur
        {
            public float XNum;
            public float YNum;
            public float XOffset;
            public float YOffset;
        }

        public Params0 CustomParams0;
        public Params1 CustomParams1;
        public Params2 CustomParams2;
        public ParamsBlur CustomParamsBlur;

        public SmUniformData Data { get; set; }

        public SmUniforms()
        {
            CustomParams0.AmbientPass = 1.0f;
            CustomParams0.LightingPass = 1.0f;

            CustomParams1.ShadowMapBias = 0.003f;
            CustomParams1.ShadowMapOffset = 0.0f;
            CustomParams1.ShadowMapParam0 = 0.0f;
            CustomParams1.ShadowMapParam1 = 1.0f;

            CustomParams2.DepthValuePow = 1.0f;
            CustomParams2.ShowSmCoverage = 1.0f;
            CustomParams2.SmTexelSize = 1.0f / 512.0f;

            CustomParamsBlur.XNum = 2.0f;
            CustomParamsBlur.YNum = 2.0f;
            CustomParamsBlur.XOffset = 10.0f / 512.0f;
            CustomParamsBlur.YOffset = 10.0f / 512.0f;

            _uParams0 = new Uniform("u_params0", UniformType.Vector4);
            _uParams1 = new Uniform("u_params1", UniformType.Vector4);
            _uParams2 = new Uniform("u_params2", UniformType.Vector4);
            _uColor = new Uniform("u_color", UniformType.Vector4);
            _uSmSamplingParams = new Uniform("u_smSamplingParams", UniformType.Vector4);
            _uCsmFarDistances = new Uniform("u_csmFarDistances", UniformType.Vector4);
            _uLightMtx = new Uniform("u_lightMtx", UniformType.Matrix4x4);

            _uTetraNormalGreen = new Uniform("u_tetraNormalGreen", UniformType.Vector4);
            _uTetraNormalYellow = new Uniform("u_tetraNormalYellow", UniformType.Vector4);
            _uTetraNormalBlue = new Uniform("u_tetraNormalBlue", UniformType.Vector4);
            _uTetraNormalRed = new Uniform("u_tetraNormalRed", UniformType.Vector4);

            _uShadowMapMtx0 = new Uniform("u_shadowMapMtx0", UniformType.Matrix4x4);
            _uShadowMapMtx1 = new Uniform("u_shadowMapMtx1", UniformType.Matrix4x4);
            _uShadowMapMtx2 = new Uniform("u_shadowMapMtx2", UniformType.Matrix4x4);
            _uShadowMapMtx3 = new Uniform("u_shadowMapMtx3", UniformType.Matrix4x4);

            _uLightPosition = new Uniform("u_lightPosition", UniformType.Vector4);
            _uLightAmbientPower = new Uniform("u_lightAmbientPower", UniformType.Vector4);
            _uLightDiffusePower = new Uniform("u_lightDiffusePower", UniformType.Vector4);
            _uLightSpecularPower = new Uniform("u_lightSpecularPower", UniformType.Vector4);
            _uLightSpotDirectionInner = new Uniform("u_lightSpotDirectionInner", UniformType.Vector4);
            _uLightAttenuationSpotOuter = new Uniform("u_lightAttenuationSpotOuter", UniformType.Vector4);

            _uMaterialKa = new Uniform("u_materialKa", UniformType.Vector4);
            _uMaterialKd = new Uniform("u_materialKd", UniformType.Vector4);
            _uMaterialKs = new Uniform("u_materialKs", UniformType.Vector4);

            Data = new SmUniformData();
        }


        public void SubmitConst()
        {
            unsafe
            {
                fixed (float* tetraNormalGreen = _tetraNormalGreen)
                {
                    Bgfx.SetUniform(_uTetraNormalGreen, tetraNormalGreen);
                }

                fixed (float* tetraNormalYellow = _tetraNormalYellow)
                {
                    Bgfx.SetUniform(_uTetraNormalYellow, tetraNormalYellow);
                }

                fixed (float* tetraNormalBlue = _tetraNormalBlue)
                {
                    Bgfx.SetUniform(_uTetraNormalBlue, tetraNormalBlue);
                }

                fixed (float* tetraNormalRed = _tetraNormalRed)
                {
                    Bgfx.SetUniform(_uTetraNormalRed, tetraNormalRed);
                }
            }
        }

        public void SubmitPerFrame()
        {
            unsafe
            {
                var params1 = CustomParams1;
                var params2 = CustomParams2;
                var paramsBlur = CustomParamsBlur;
                Bgfx.SetUniform(_uParams1, (float*) &params1);
                Bgfx.SetUniform(_uParams2, (float*) &params2);
                Bgfx.SetUniform(_uSmSamplingParams, (float*) &paramsBlur);
                
                fixed (float* csmFarDistancePtr = _csmFarDistances)
                {
                    Bgfx.SetUniform(_uCsmFarDistances, csmFarDistancePtr);
                }

                var material = Data.Material;
                var ambient = material.Ambient;
                var diffuse = material.Diffuse;
                var specular = material.Specular;
                Bgfx.SetUniform(_uMaterialKa, (float*) &ambient);
                Bgfx.SetUniform(_uMaterialKd, (float*) &diffuse);
                Bgfx.SetUniform(_uMaterialKs, (float*) &specular);

                var light = Data.Light;
                var positionViewspace = light.PositionViewSpace;
                var ambientPower = light.Ambient;
                var diffusePower = light.Diffuse;
                var specularPower = light.Specular;
                var spotDirectionInnerViewport = light.SpotDirectionInnerViewSpace;
                var attenuationSpotOuter = light.AttenuationSpotOuter;
                Bgfx.SetUniform(_uLightPosition, (float*) &positionViewspace);
                Bgfx.SetUniform(_uLightAmbientPower, (float*) &ambientPower);
                Bgfx.SetUniform(_uLightDiffusePower, (float*) &diffusePower);
                Bgfx.SetUniform(_uLightSpecularPower, (float*) &specularPower);
                Bgfx.SetUniform(_uLightSpotDirectionInner, (float*) &spotDirectionInnerViewport);
                Bgfx.SetUniform(_uLightAttenuationSpotOuter, (float*) &attenuationSpotOuter);
            }
        }

        public void SubmitPerDraw()
        {
            unsafe
            {
                var mtx0 = Data.SmTransforms[0];
                var mtx1 = Data.SmTransforms[1];
                var mtx2 = Data.SmTransforms[2];
                var mtx3 = Data.SmTransforms[3];
                Bgfx.SetUniform(_uShadowMapMtx0, (float*) &mtx0);
                Bgfx.SetUniform(_uShadowMapMtx1, (float*) &mtx1);
                Bgfx.SetUniform(_uShadowMapMtx2, (float*) &mtx2);
                Bgfx.SetUniform(_uShadowMapMtx3, (float*) &mtx3);

                var params0 = CustomParams0;
                Bgfx.SetUniform(_uParams0, (float*) &params0);

                var lightMtx = Data.LightTransform;
                var color = Data.Color;
                Bgfx.SetUniform(_uLightMtx, (float*) &lightMtx);
                Bgfx.SetUniform(_uColor, (float*) &color);
            }
        }

        public void Dispose()
        {
            _uParams0.Dispose();
            _uParams1.Dispose();
            _uParams2.Dispose();
            _uColor.Dispose();
            _uSmSamplingParams.Dispose();
            _uCsmFarDistances.Dispose();

            _uTetraNormalGreen.Dispose();
            _uTetraNormalYellow.Dispose();
            _uTetraNormalBlue.Dispose();
            _uTetraNormalRed.Dispose();

            _uShadowMapMtx0.Dispose();
            _uShadowMapMtx1.Dispose();
            _uShadowMapMtx2.Dispose();
            _uShadowMapMtx3.Dispose();

            _uLightMtx.Dispose();
            _uLightPosition.Dispose();
            _uLightAmbientPower.Dispose();
            _uLightDiffusePower.Dispose();
            _uLightSpecularPower.Dispose();
            _uLightSpotDirectionInner.Dispose();
            _uLightAttenuationSpotOuter.Dispose();

            _uMaterialKa.Dispose();
            _uMaterialKd.Dispose();
            _uMaterialKs.Dispose();
        }
    }

    public class SmPrograms : IDisposable
    {
        private Program _black;
        public Program Texture;
        public Program ColorTexture;
        public Program[] VBlur = new Program[(int) SmPackDepth.Count];
        public Program[] HBlur = new Program[(int) SmPackDepth.Count];
        public Program[] DrawDepth = new Program[(int) SmPackDepth.Count];
        public Program[,] PackDepth = new Program[(int) SmDepthImpl.Count, (int) SmPackDepth.Count]; //<-ok (2)

        public Program[,,] ColorLighting =
            new Program[(int) SmType.Count, (int) SmDepthImpl.Count, (int) SmImpl.Count];

        public SmPrograms()
        {
            _black = ResourceLoader.LoadProgram("shadowmaps_color", "shadowmaps_color_black");
            Texture = ResourceLoader.LoadProgram("shadowmaps_texture", "shadowmaps_texture");
            ColorTexture = ResourceLoader.LoadProgram("shadowmaps_color_texture", "shadowmaps_color_texture");

            // Blur.
            VBlur[(int) SmPackDepth.VSM] = ResourceLoader.LoadProgram("shadowmaps_vblur", "shadowmaps_vblur_vsm");
            HBlur[(int) SmPackDepth.VSM] = ResourceLoader.LoadProgram("shadowmaps_hblur", "shadowmaps_hblur_vsm");

            // Draw depth.
            DrawDepth[(int) SmPackDepth.VSM] =
                ResourceLoader.LoadProgram("shadowmaps_unpackdepth", "shadowmaps_unpackdepth_vsm");

            // Pack depth.
            PackDepth[(int) SmDepthImpl.InvZ, (int) SmPackDepth.VSM] =
                ResourceLoader.LoadProgram("shadowmaps_packdepth", "shadowmaps_packdepth_vsm");

            // Color lighting.
            ColorLighting[(int) SmType.Single, (int) SmDepthImpl.InvZ, (int) SmImpl.VSM] =
                ResourceLoader.LoadProgram("shadowmaps_color_lighting", "shadowmaps_color_lighting_vsm");

            ColorLighting[(int) SmType.Omni, (int) SmDepthImpl.InvZ, (int) SmImpl.VSM] =
                ResourceLoader.LoadProgram("shadowmaps_color_lighting_omni", "shadowmaps_color_lighting_vsm_omni");

            ColorLighting[(int) SmType.Cascade, (int) SmDepthImpl.InvZ, (int) SmImpl.VSM] =
                ResourceLoader.LoadProgram("shadowmaps_color_lighting_csm", "shadowmaps_color_lighting_vsm_csm");
        }

        public void Dispose()
        {
            for (var i = 0; i < (int) SmType.Count; i++)
            {
                for (var j = 0; j < (int) SmDepthImpl.Count; j++)
                {
                    for (var k = 0; k < (int) SmImpl.Count; k++)
                    {
                        ColorLighting[i, j, k].Dispose();
                    }
                }
            }

            // Pack depth.
            for (var i = 0; i < (int) SmDepthImpl.Count; i++)
            {
                for (var j = 0; j < (int) SmPackDepth.Count; j++)
                {
                    PackDepth[i, j].Dispose();
                }
            }

            // Draw depth.
            for (var i = 0; i < (int) SmPackDepth.Count; i++)
            {
                DrawDepth[i].Dispose();
            }

            // Hblur.
            for (var i = 0; i < (int) SmPackDepth.Count; i++)
            {
                HBlur[i].Dispose();
            }

            // Vblur.
            for (var i = 0; i < (int) SmPackDepth.Count; i++)
            {
                VBlur[i].Dispose();
            }

            _black.Dispose();
            Texture.Dispose();
            ColorTexture.Dispose();
        }
    }

    public enum SmRenderStateGroupType
    {
        Default = 0,

        SmPackDepth = 1,
        SmPackDepthHoriz = 2,
        SmPackDepthVert = 3,

        CustomBlendLightTexture = 4,
        CustomDrawPlaneBottom = 5,

        Count = 6
    };


    public class ShadowMapManager
    {
        public static readonly List<RenderStateGroup> StateGroups = new List<RenderStateGroup>
        {
            {
                // Default
                new RenderStateGroup(
                    RenderState.WriteRGB
                    | RenderState.WriteA
                    | RenderState.WriteZ
                    | RenderState.DepthTestLess
                    | RenderState.CullClockwise
                    | RenderState.Multisampling
                    , uint.MaxValue
                    , StencilFlags.None
                    , StencilFlags.None
                )
            },
            {
                // ShadowMap_PackDepth
                new RenderStateGroup(
                    RenderState.WriteRGB
                    | RenderState.WriteA
                    | RenderState.WriteZ
                    | RenderState.DepthTestLess
                    | RenderState.CullClockwise
                    | RenderState.Multisampling
                    , uint.MaxValue
                    , StencilFlags.None
                    , StencilFlags.None
                )
            },
            {
                // ShadowMap_PackDepthHoriz
                new RenderStateGroup(
                    RenderState.WriteRGB
                    | RenderState.WriteA
                    | RenderState.WriteZ
                    | RenderState.DepthTestLess
                    | RenderState.CullCounterclockwise
                    | RenderState.Multisampling,
                    uint.MaxValue,
                    StencilFlags.TestEqual
                    | StencilFlags.ReferenceValue(1)
                    | StencilFlags.ReadMask(0xff)
                    | StencilFlags.FailSKeep
                    | StencilFlags.FailZKeep
                    | StencilFlags.PassZKeep,
                    StencilFlags.None)
            },
            {
                // ShadowMap_PackDepthVert
                new RenderStateGroup(
                    RenderState.WriteRGB
                    | RenderState.WriteA
                    | RenderState.WriteZ
                    | RenderState.DepthTestLess
                    | RenderState.CullCounterclockwise
                    | RenderState.Multisampling,
                    uint.MaxValue,
                    StencilFlags.TestEqual
                    | StencilFlags.ReferenceValue(0)
                    | StencilFlags.ReadMask(0xff)
                    | StencilFlags.FailSKeep
                    | StencilFlags.FailZKeep
                    | StencilFlags.PassZKeep,
                    StencilFlags.None)
            },
            {
                // Custom_BlendLightTexture
                new RenderStateGroup(
                    RenderState.WriteRGB
                    | RenderState.WriteA
                    | RenderState.WriteZ
                    | RenderState.DepthTestLess
                    | RenderState.BlendFunction(RenderState.BlendSourceColor,
                        RenderState.BlendInverseSourceColor)
                    | RenderState.CullClockwise
                    | RenderState.Multisampling,
                    uint.MaxValue,
                    StencilFlags.None,
                    StencilFlags.None)
            },
            {
                // Custom_DrawPlaneBottom
                new RenderStateGroup(
                    RenderState.WriteRGB
                    | RenderState.CullCounterclockwise
                    | RenderState.Multisampling
                    | RenderState.DepthTestLess,
                    uint.MaxValue,
                    StencilFlags.None,
                    StencilFlags.None)
            },
        };

        public readonly SmPrograms Programs;
        public readonly SmUniforms Uniforms;

        public readonly SmSettings[,,] Settings;


        public ShadowMapManager()
        {
            Programs = new SmPrograms();
            Uniforms = new SmUniforms();

            Settings = new SmSettings[(int) LightType.Count, (int) SmDepthImpl.Count, (int) SmImpl.Count];
        }
    }
}