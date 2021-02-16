using System;
using System.Numerics;

namespace SaffronEngine.Rendering
{
    public enum LightType
    {
        Point,
        Spot,
        Directional,
        Count
    }

    public enum LightProjType
    {
        Horizontal,
        Vertical,
        Count
    };

    public class Light
    {
        public struct RgbPower
        {
            public float R;
            public float G;
            public float B;
            public float Power;

            public RgbPower(float r, float g, float b, float power)
            {
                R = r;
                G = g;
                B = b;
                Power = power;
            }
        };

        public struct SpotInner
        {
            public float X;
            public float Y;
            public float Z;
            public float Inner;

            public SpotInner(float x, float y, float z, float inner)
            {
                X = x;
                Y = y;
                Z = z;
                Inner = inner;
            }
        };

        public struct SpotOuter
        {
            public float Constant;
            public float Linear;
            public float Quadratic;
            public float Outer;

            public SpotOuter(float constant, float linear, float quadratic, float outer)
            {
                Constant = constant;
                Linear = linear;
                Quadratic = quadratic;
                Outer = outer;
            }
        };


        public Vector4 Position;
        public RgbPower Ambient;
        public RgbPower Diffuse;
        public RgbPower Specular;
        public SpotInner SpotDirectionInner;
        public SpotOuter AttenuationSpotOuter;

        public Vector4 SpotDirectionInnerViewSpace;
        public Vector4 PositionViewSpace;

        public Light()
        {
        }

        public Light(Vector4 position, RgbPower ambient, RgbPower diffuse, RgbPower specular,
            SpotInner spotDirectionInner, SpotOuter attenuationSpotOuter)
        {
            Position = position;
            Ambient = ambient;
            Diffuse = diffuse;
            Specular = specular;
            SpotDirectionInner = spotDirectionInner;
            AttenuationSpotOuter = attenuationSpotOuter;
        }


        public void ComputeSpaceViewComponents(Matrix4x4 view)
        {
            PositionViewSpace = Vector4.Transform(Position, view);

            var tmp = new Vector4(
                SpotDirectionInner.X,
                SpotDirectionInner.Y,
                SpotDirectionInner.Z,
                0.0f
            );

            SpotDirectionInnerViewSpace = Vector4.Transform(tmp, view);
            SpotDirectionInnerViewSpace.W = SpotDirectionInner.Inner;
        }
    }
}