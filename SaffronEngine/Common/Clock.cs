using System.Diagnostics;

namespace SaffronEngine.Common
{
    public class Clock
    {
        private readonly long frequency;
        private long lastFrame;
        private long initialTick;

        public Clock()
        {
            frequency = Stopwatch.Frequency;
        }

        public void Start()
        {
            initialTick = Stopwatch.GetTimestamp();
        }

        public Time Frame { get; private set; }

        public Time Elapsed
        {
            get
            {
                var tick = Stopwatch.GetTimestamp();
                var elapsed = Time.FromSeconds(((float) (tick - lastFrame)) / frequency);
                return elapsed;
            }
        }

        public Time Restart()
        {
            var tick = Stopwatch.GetTimestamp();
            var elapsed = Time.FromSeconds(((float) (tick - lastFrame)) / frequency);
            Frame = elapsed;
            lastFrame = tick;
            return Frame;
        }

        public Time TotalTime
        {
            get
            {
                var tick = Stopwatch.GetTimestamp();
                return Time.FromSeconds(((float) (tick - initialTick)) / frequency);
            }
        }
    }
}