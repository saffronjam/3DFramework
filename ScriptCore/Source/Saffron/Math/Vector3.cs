using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;

namespace Se
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float X;
        public float Y;
        public float Z;

        public Vector3(float scalar)
        {
            X = Y = Z = scalar;
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3(Vector2 vector)
        {
            X = vector.X;
            Y = vector.Y;
            Z = 0.0f;
        }

        public Vector3(Vector4 vector)
        {
            X = vector.X;
            Y = vector.Y;
            Z = vector.Z;
        }
        public static Vector3 operator -(Vector3 vector) => new Vector3(-vector.X, -vector.Y, -vector.Z);

        public void Clamp(Vector3 min, Vector3 max)
        {
            if (X < min.X)
                X = min.X;
            if (X > max.X)
                X = max.X;

            if (Y < min.Y)
                Y = min.Y;
            if (Y > max.Y)
                Y = max.Y;

            if (Z < min.Z)
                Z = min.Z;
            if (Z > max.Z)
                Z = max.Z;
        }

        public Vector2 XY
        {
            get => new Vector2(X, Y);
            set { X = value.X; Y = value.Y; }
        }
        public Vector2 XZ
        {
            get => new Vector2(X, Z);
            set { X = value.X; Z = value.Y; }
        }
        public Vector2 YZ
        {
            get => new Vector2(Y, Z);
            set { Y = value.X; Z = value.Y; }
        }


    }
}
