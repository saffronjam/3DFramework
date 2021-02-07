using System;
using System.Collections.Generic;
using System.Drawing;
using System.Numerics;
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

        private void RenderThread()
        {
            ///////////////////////////////////////////////////////
            // These instructions must run in the renderer thread
            ///////////////////////////////////////////////////////
            Bgfx.Init();

            SceneRenderer = new SceneRenderer();
            _gui = new Gui();

            Bgfx.SetDebugFeatures(DebugFeatures.DisplayText);

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

                _gui.Begin();
                splashScreenPane.OnUpdate();
                splashScreenPane.OnGuiRender();
                Window.DispatchEvents();
                _gui.End();
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