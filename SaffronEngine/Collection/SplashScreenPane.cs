using System;
using System.Numerics;
using ImGuiNET;
using Renderer;
using SaffronEngine.Common;

namespace SaffronEngine.Collection
{
    public class SplashScreenPane
    {
        private readonly string _title = "Saffron Engine";

        public Batch Batch { get; private set; }

        private readonly Texture _texture;
        private readonly IntPtr _uiTextureHandle;
        private const string _finalizingStatus = "Finilizing";

        private Time _progressTimer = Time.Zero;
        private float _progressView;
        private float _progressViewFinished;

        private Time _holdTimer = Time.Zero;
        private readonly Time _holdTimerFinished = Time.FromSeconds(0.8f);

        private readonly FadePane _fadeIn = new FadePane(FadePane.Type.In, Time.FromSeconds(0.4f),
            Time.FromSeconds(0.5f), true);

        private readonly FadePane _fadeOut = new FadePane(FadePane.Type.Out, Time.FromSeconds(0.4f),
            (Time timer, Time duration) =>
                Math.Min(duration.AsSeconds(), timer.AsSeconds() * 2.0f) / duration.AsSeconds() * 255.0f);

        public bool Idle { get; private set; }
        public bool Finished { get; private set; }
        public bool Hidden { get; private set; }
        public void Show() => Hidden = false;
        public void Hide() => Hidden = true;

        public SplashScreenPane(Batch batch)
        {
            Batch = batch;
            _texture = ResourceLoader.LoadTexture("Saffron.dds");
            _uiTextureHandle = Gui.AddTexture(_texture);

            _fadeOut.Finished += (sender, args) => Finished = true;
        }

        public void OnUpdate()
        {
            if (Hidden)
                return;

            var dt = Global.Clock.Frame;

            _fadeIn.OnUpdate();

            if (_fadeOut.Active && !_fadeIn.Active)
            {
                if (_holdTimer >= _holdTimerFinished)
                {
                    _fadeOut.OnUpdate();
                }
                else
                {
                    _holdTimer += dt;
                }
            }

            if (Batch.Progress > 99.9f)
            {
                _fadeOut.Start();
            }

            if (Math.Abs(Batch.Progress - _progressViewFinished) > 0.1f)
            {
                _progressViewFinished = Batch.Progress;
                _progressTimer = Time.Zero;
            }
            else
            {
                _progressTimer += dt;
            }

            _progressView += (_progressViewFinished - _progressView) *
                             (float) Math.Sin(_progressTimer.AsSeconds() / (2.0f * Math.PI));
            GenUtils.Clamp(ref _progressView, 0.0f, 100.0f);
        }

        public void OnGuiRender()
        {
            if (Hidden)
                return;

            var viewport = ImGui.GetMainViewport();
            ImGui.SetNextWindowPos(viewport.Pos);
            ImGui.SetNextWindowSize(viewport.Size);

            ImGui.Begin("Loading Screen##", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize);


            ImGui.GetWindowDrawList().AddRectFilled(Vector2.Zero, ImGui.GetWindowSize(),
                ImGui.GetColorU32(new Vector4(0, 0, 0, 1)));

            var windowSize = ImGui.GetWindowSize();

            const int logoWidth = 200;
            const int logoHeight = 200;
            ImGui.SetCursorPos(new Vector2(
                windowSize.X / 2.0f - logoWidth / 2.0f,
                2.0f * windowSize.Y / 5.0f - logoHeight / 2.0f
            ));
            ImGui.Image(_uiTextureHandle, new Vector2(logoWidth, logoHeight), Vector2.Zero, Vector2.One);

            Gui.SetFontSize(36);
            ImGui.NewLine();
            Gui.SetFontSize(48);

            var titleTextWidth = ImGui.CalcTextSize(_title).X;
            ImGui.SetCursorPosX(ImGui.GetWindowWidth() / 2.0f - titleTextWidth / 2.0f);
            ImGui.Text(_title);

            Gui.SetFontSize(24);

            var progressAsString =  _progressView.ToString("0") + "%%";

            ImGui.NewLine();
            var currentProgressTextWidth = ImGui.CalcTextSize(progressAsString).X;
            ImGui.SetCursorPosX(ImGui.GetWindowWidth() / 2.0f - currentProgressTextWidth / 2.0f);
            ImGui.Text(progressAsString);

            Gui.SetFontSize(18);

            var status = _progressViewFinished < 100.0f ? Batch.CurrentDescription : _finalizingStatus;
            if (!string.IsNullOrEmpty(status))
            {
                var infoTextWidth = ImGui.CalcTextSize(status).X;
                ImGui.NewLine();
                ImGui.SetCursorPosX(windowSize.X / 2.0f - infoTextWidth / 2.0f);
                ImGui.Text(status);
            }

            _fadeIn.OnGuiRender();
            _fadeOut.OnGuiRender();

            ImGui.End();
        }
    };
}