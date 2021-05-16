using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Saffron
{
    public class Camera
    {
        public enum ProjectionMode : uint
        {
            Perspective, Orthographic
        }

        public Camera(uint viewportWidth, uint viewportHeight, ProjectionMode mode)
        {
            m_UnmanagedInstance = Constructor_Native(viewportWidth, viewportHeight, (uint)mode);
        }

        internal Camera(IntPtr unmanagedInstance)
        {
            m_UnmanagedInstance = unmanagedInstance;
        }
        ~Camera()
        {
            Destructor_Native(m_UnmanagedInstance);
        }

        public ProjectionMode Projection
        {
            get
            {
                return (ProjectionMode)GetProjectionMode_Native(m_UnmanagedInstance);
            }
            set
            {
                SetProjectionMode_Native(m_UnmanagedInstance, (uint)value);
            }
        }

        internal IntPtr m_UnmanagedInstance;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr Constructor_Native(uint viewportWidth, uint viewportHeight, uint mode);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Destructor_Native(IntPtr unmanagedInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint GetProjectionMode_Native(IntPtr unmanagedInstance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetProjectionMode_Native(IntPtr unmanagedInstance, uint mode);
    }
}
