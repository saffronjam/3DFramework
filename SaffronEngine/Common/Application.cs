using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Threading;
using ImGuiNET;
using Renderer;
using SaffronEngine.Collection;
using SaffronEngine.Exceptions;
using SaffronEngine.Platform;
using SaffronEngine.Platform.Windows;
using SaffronEngine.Rendering;
using Serilog;
using Serilog.Core;

namespace SaffronEngine.Common
{
    public abstract class Application
    {
        private static Application _instance;
        public Window Window { get; private set; }
        public SceneRenderer SceneRenderer { get; private set; }

        private Thread _thread;
        private readonly Batch _attachBatch = new Batch();
        private readonly FadePane _fadeIn = new FadePane(FadePane.Type.In, Time.FromSeconds(0.5f));

        private readonly List<Layer> _layers = new List<Layer>();
        private Gui _gui;
        private readonly Dockspace _dockspace = new Dockspace();

        private bool _shouldRun = true;
        private bool _wantRenderTargetResize = true;


        public Application(string title, uint width, uint height)
        {
            Log.Logger = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.Console()
                .CreateLogger();

            Thread.CurrentThread.Name = "OS Thread";

            Window = new WindowsWindow(title, width, height);
            Window.Show();
            Window.Closed += (sender, args) => { Exit(); };
            Window.Resized += (sender, args) => { _wantRenderTargetResize = true; };

            _instance = this;

            Input.AddEventSource(Window);

            Bgfx.SetWindowHandle(Window.Handle);
        }

        public void Run()
        {
            _thread = new Thread(() =>
            {
                RenderThread();
                Window.Close();
            }) {Name = "Renderer Thread"};
            _thread.Start();

            // run the native OS message loop on this thread
            // this blocks until the window closes and the loop exits
            Window.RunMessageLoop();

            // wait for the render thread to finish
            _thread.Join();
        }

        public void Exit()
        {
            _shouldRun = false;
        }

        class CallbackHandler : ICallbackHandler
        {
            public void ReportError(string fileName, int line, ErrorType errorType, string message)
            {
                // Log.Debug("({0}:{1}) {2}: {3}", fileName, line, errorType, message);
            }

            public void ReportDebug(string fileName, int line, string format, IntPtr args)
            {
            }

            public int GetCachedSize(long id)
            {
                var file = GetCacheFile(id);
                if (!file.Exists)
                    return 0;

                return (int) file.Length;
            }

            public bool GetCacheEntry(long id, IntPtr data, int size)
            {
                var file = GetCacheFile(id);
                if (!file.Exists)
                    return false;

                var bytes = new byte[size];
                using (var stream = file.OpenRead())
                {
                    var read = stream.Read(bytes, 0, size);
                    if (read != size)
                        return false;
                }

                // we could avoid this extra copy with some more work, but I'm lazy
                Marshal.Copy(bytes, 0, data, size);
                return true;
            }

            public void SetCacheEntry(long id, IntPtr data, int size)
            {
              
            }

            public void SaveScreenShot(string path, int width, int height, int pitch, IntPtr data, int size,
                bool flipVertical)
            {
               
            }

            public void CaptureStarted(int width, int height, int pitch, TextureFormat format, bool flipVertical)
            {
            }

            public void CaptureFrame(IntPtr data, int size)
            {
            }

            public void CaptureFinished()
            {
            }

            public void ProfilerBegin(string name, int color, string filePath, int line)
            {
            }

            public void ProfilerEnd()
            {
            }

            static FileInfo GetCacheFile(long id)
            {
                // we use the cache id as the filename, and just dump in the current directory
                return new FileInfo(Path.Combine("bin", id.ToString("x")));
            }
        }


        private void RenderThread()
        {
            ///////////////////////////////////////////////////////
            // These instructions must run in the renderer thread
            ///////////////////////////////////////////////////////
            Bgfx.Init(new InitSettings
            {
                CallbackHandler = new CallbackHandler()
            });
            Bgfx.Reset(1280, 720, ResetFlags.Vsync);

            SceneRenderer = new SceneRenderer();
            _gui = new Gui();

            Bgfx.SetDebugFeatures(DebugFeatures.DisplayText);
            Bgfx.DebugTextWrite(0, 1, DebugColor.White, DebugColor.Blue, "SharpBgfx/Samples/00-HelloWorld");
            

            OnInit();
            while (_shouldRun)
            {
                Global.Clock.Restart();

                if (!_attachBatch.Done)
                {
                    RunSplashScreen();
                    _fadeIn.Start();
                }

                Input.OnUpdate();
                Window.DispatchEvents();

                _gui.Begin();
                _dockspace.Begin();

                OnUpdate();
                foreach (var layer in _layers)
                {
                    layer.OnUpdate();
                }

                foreach (var layer in _layers)
                {
                    layer.OnGuiRender();
                }

                _fadeIn.OnUpdate();
                _fadeIn.OnGuiRender();

                _dockspace.End();
                _gui.End();

                Bgfx.Frame();
            }

            Bgfx.Shutdown();
        }

        public static Application Instance
        {
            get
            {
                if (_instance != null)
                {
                    return _instance;
                }

                throw new SaffronStateException("Application not created.");
            }
        }

        public abstract void OnInit();

        public abstract void OnUpdate();
        public abstract void OnGuiRender();

        public void PushLayer(Layer layer)
        {
            _layers.Add(layer);
            layer.OnAttach(_attachBatch);
        }

        public void PopLayer()
        {
            _layers[^1].OnDetach();
            _layers.RemoveAt(_layers.Count - 1);
        }

        public void RemoveLayer(Layer layer)
        {
            layer.OnDetach();
            _layers.Remove(layer);
        }

        private void RunSplashScreen()
        {
            _attachBatch.Execute();

            var splashScreenPane = new SplashScreenPane(_attachBatch);
            while (!splashScreenPane.Finished)
            {
                if (_wantRenderTargetResize)
                {
                    Bgfx.Reset((int) Window.Width, (int) Window.Height, ResetFlags.Vsync);
                    _wantRenderTargetResize = false;
                }

                // _gui.Begin();
                splashScreenPane.OnUpdate();
                // splashScreenPane.OnGuiRender();
                Window.DispatchEvents();
                // _gui.End();
                Schedule.Execute();
                Global.Clock.Restart();
                var step = Global.Clock.Frame.AsSeconds();
                var duration = splashScreenPane.Batch.Done ? 0 : System.Math.Max(0, (int) (1000.0f / 60.0f - step));
                Thread.Sleep(duration);
                Bgfx.Frame();
            }
        }
    }
}