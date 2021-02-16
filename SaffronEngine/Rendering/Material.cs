using System.Drawing;
using System.Numerics;

namespace SaffronEngine.Rendering
{
    public class Material
    {
        public Vector4 Ambient { get; set; }
        public Vector4 Diffuse { get; set; }
        public Vector4 Specular { get; set; }

        public Material()
        {
        }

        public Material(Vector4 ambient, Vector4 diffuse, Vector4 specular)
        {
            Ambient = ambient;
            Diffuse = diffuse;
            Specular = specular;
        }
    }
}