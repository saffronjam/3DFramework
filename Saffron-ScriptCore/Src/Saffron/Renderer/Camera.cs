using Se;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Se
{
    public class Camera
    {
        public enum ProjectionMode : uint
        {
            Orthographic, Projection
        }

        public Camera(uint ViewportWidth, uint ViewportHeight, ProjectionMode Mode)
        {
            m_UnmanagedInstance = Constructor_Native(ViewportWidth, ViewportHeight, Mode);
        }

        internal Camera(IntPtr unmanagedInstance)
        {
            m_UnmanagedInstance = unmanagedInstance;
        }
        ~Camera()
        {
            Destructor_Native(m_UnmanagedInstance);
        }

        internal IntPtr m_UnmanagedInstance;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr Constructor_Native(uint ViewportWidth, uint ViewportHeight, ProjectionMode Mode);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Destructor_Native(IntPtr unmanagedInstance);
    }
}
