using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace SaffronEngine.Common
{
    public class Batch
    {
        public class Submition
        {
            public readonly Action Action;
            public readonly string Description;

            public Submition(Action action, string description)
            {
                Action = action;
                Description = description;
            }
        }

        private readonly List<Submition> _submitions;
        private Thread _worker;
        private bool _shouldQuit = false;

        public float Progress { get; set; }
        public int NoJobs => _submitions.Count;
        public string CurrentDescription { get; private set; }
        public bool Done { get; private set; }

        public Batch()
        {
            _submitions = new List<Submition>();
        }

        public void Sumbit(Submition submition)
        {
            _submitions.Add(submition);
        }

        public void Sumbit(Action action, string description)
        {
            _submitions.Add(new Submition(action, description));
        }

        public void Execute()
        {
            _shouldQuit = false;
            _worker = new Thread(ExecuteThreadFn);
            _worker.Start();
        }

        public void Reset()
        {
            ForceQuit();
            Done = false;
            Progress = 0.0f;
            _shouldQuit = false;
        }

        public void ForceQuit()
        {
            _shouldQuit = true;
            _worker.Join();
            _shouldQuit = false;
        }

        private void ExecuteThreadFn()
        {
            Done = false;
            Progress = 0.0f;
            var currentJob = 0;
            foreach (var submition in _submitions.TakeWhile(submition => !_shouldQuit))
            {
                submition.Action();
                CurrentDescription = submition.Description;
                Progress += ++currentJob * (100.0f / NoJobs);
            }

            _submitions.Clear();
            Progress = 100.0f;
            Done = true;
        }
    }
}