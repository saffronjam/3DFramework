using System.Numerics;
using Serilog;

namespace SaffronEngine.Common
{
    public class Camera
    {
        protected bool _wantNewView = true;
        private Vector2 _viewportSize = Vector2.One;
        public Matrix4x4 Projection { get; set; }
        public float Exposure { get; set; }

        public Vector2 ViewportSize
        {
            get => _viewportSize;
            set
            {
                _viewportSize = value;
                _wantNewView = true;
            }
        }

        public Camera(Matrix4x4 projection)
        {
            Projection = projection;
            Exposure = 1.0f;
            ViewportSize = Vector2.One;
        }
    }
}