namespace SaffronEngine.Common
{
    public static class Global
    {
        public static class Clock
        {
            private static readonly Common.Clock _inst = new Common.Clock();

            public static void Start()
            {
                _inst.Start();
            }

            public static Time Frame => _inst.Frame;
            public static Time Elapsed => _inst.Elapsed;

            public static Time Restart()
            {
                return _inst.Restart();
            }

            public static Time TotalTime => _inst.TotalTime;
        }
    }
}