using System;

namespace SaffronEngine.Exceptions
{
    public class SaffronStateException : InvalidOperationException
    {
        public SaffronStateException(string message) : base(message)
        {
        }
    }
}