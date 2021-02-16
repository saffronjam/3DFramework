using System;

namespace SaffronEngine.Common
{
    static class GenUtils
    {
        public static T Clamped<T>(T value, T min, T max) where T : IComparable<T>
        {
            if (value.CompareTo(min) < 0)
                return min;

            if (value.CompareTo(max) > 0)
                return max;

            return value;
        }

        public static void Clamp<T>(ref T value, T min, T max) where T : IComparable<T>
        {
            if (value.CompareTo(min) < 0)
                value = min;

            if (value.CompareTo(max) > 0)
                value = max;
        }

        public static byte Lerp(byte start, byte end, float amount)
        {
            return (byte) (start + (byte) (amount * (end - start)));
        }

        public static float Lerp(float start, float end, float amount)
        {
            return start + amount * (end - start);
        }

        public static uint PackF4u(float x, float y, float z)
        {
            var bytes = new byte[]
            {
                (byte) (x * 127.0f + 128.0f),
                (byte) (y * 127.0f + 128.0f),
                (byte) (z * 127.0f + 128.0f),
                128
            };
            return BitConverter.ToUInt32(bytes, 0);
        }

        public static float ToRadians(float degrees)
        {
            return (float) (degrees * Math.PI / 180.0);
        }
    }
}