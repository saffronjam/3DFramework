using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Se
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode key)
        {
            return IsKeyDown_Native(key);
        }

        public static bool IsKeyPressed(KeyCode key)
        {
            return IsKeyPressed_Native(key);
        }

        public static bool IsMouseButtonPressed(MouseButtonCode mouseButton)
        {
            return IsMouseButtonPressed_Native(mouseButton);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyDown_Native(KeyCode key);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyPressed_Native(KeyCode key);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsMouseButtonPressed_Native(MouseButtonCode mouseButton);

    }
}
