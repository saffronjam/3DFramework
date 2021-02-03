using System;
using System.Drawing;
using System.Numerics;
using ImGuiNET;
using SaffronEngine.Common;
using Serilog;

namespace SaffronEngine.Collection
{
    public class FadePane
    {
        public enum Type
        {
            Out = 0,
            In = 1
        };

        private readonly Type _type;
        private readonly Func<Time, Time, float> _alphaFunction;
        private bool _wantFade = false;
        private Color _fadeColor;
        private Time _timer = Time.Zero;
        private readonly Time _duration;
        private Time _delayTimer = Time.Zero;
        private readonly Time _delay;


        public FadePane(Type type,
            Time duration,
            Time delay = default,
            bool startOnCreation = false,
            Color color = default)
        {
            _type = type;
            _duration = duration;
            _delay = delay;
            _wantFade = startOnCreation;
            _fadeColor = color;

            _alphaFunction = _type switch
            {
                Type.In => DefaultInAlphaFunction,
                Type.Out => DefaultOutAlphaFunction,
                _ => _alphaFunction
            };
        }

        public FadePane(Type type,
            Time duration,
            Func<Time, Time, float> alphaFunction,
            Time delay = default,
            bool startOnCreation = false,
            Color color = default)
        {
            _type = type;
            _duration = duration;
            _delay = delay;
            _wantFade = startOnCreation;
            _fadeColor = color;
            _alphaFunction = alphaFunction;
        }

        public void OnUpdate()
        {
            _wantFade = false;
            Finished?.Invoke(this, EventArgs.Empty);
            return;
            if (!_wantFade) return;
            
            var dt = Global.Clock.Frame;
            _delayTimer += dt;
                
            if (_delayTimer < _delay) return;
            _delayTimer = _delay;
            _timer += dt;
                
            if (_timer < _duration) return;
            _timer = _duration;
            _wantFade = false;
            Finished?.Invoke(this, EventArgs.Empty);
        }

        public void OnGuiRender()
        {
            return;
            var alpha = _alphaFunction(_timer, _duration);
            var color = ImGui.GetColorU32(new Vector4(0, 0, 0, alpha));
            var position = new Vector2(0.0f, 0.0f);
            var size = Application.Instance.Window.Size;
            var imSize = new Vector2((float) size.X, (float) size.Y);
            ImGui.GetWindowDrawList().AddRectFilled(position, imSize, color);
        }

        public void Start()
        {
            _wantFade = true;
        }

        public bool Active => _wantFade;

        private static float DefaultInAlphaFunction(Time timer, Time duration)
        {
            return GenUtils.Clamped(1.0f - timer / duration, 0.0f, 1.0f);
        }

        private static float DefaultOutAlphaFunction(Time timer, Time duration)
        {
            return GenUtils.Clamped(timer / duration, 0.0f, 1.0f);
        }

        /// <summary>Event handler for the Finished event</summary>
        public event EventHandler Finished = null;
    };
}