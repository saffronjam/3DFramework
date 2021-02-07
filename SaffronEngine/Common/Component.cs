
using System.Numerics;

namespace SaffronEngine.Common
{
    public interface ISaffronComponent
    {
    }

    public class Component
    {
        public class Mesh : ISaffronComponent
        {
            public Common.Mesh Handle { get; set; }
        }
        
        public class Transform : ISaffronComponent
        {
            public Matrix4x4 Matrix { get; set; }

            public Transform()
            {
                Matrix = Matrix4x4.Identity;
            }
        }
    }
}