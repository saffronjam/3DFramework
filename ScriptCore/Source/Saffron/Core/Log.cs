using System;
using System.Runtime.CompilerServices;

namespace Saffron
{
    public class Log
    {
        public static void Info(string message)
        {
            LogInfo_Native(message);
        }

        public static void Info(object obj)
        {
            Info(obj.ToString());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void LogInfo_Native(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void LogWarn_Native(string message);
    }
}