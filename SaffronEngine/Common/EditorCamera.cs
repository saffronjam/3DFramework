using System;
using System.Numerics;
using ImGuiNET;
using Serilog;

namespace SaffronEngine.Common
{
    public class EditorCamera : Camera
    {
        public enum ControllerStyle
        {
            Game = 0,
            Maya = 1
        };

        private Vector3 _position;
        private Vector3 _rotation;
        private float _pitch;
        private float _yaw;
        private Vector2 _lastSwipe;
        private readonly Vector3 _worldUp = new Vector3(0.0f, 1.0f, 0.0f);

        public EditorCamera() :
            this(Matrix4x4.CreatePerspectiveFieldOfView((float) Math.PI / 4.0f, 4.0f / 3.0f, 0.1f, 10000.0f))
        {
        }

        public EditorCamera(Matrix4x4 projection) : base(projection)
        {
            Enabled = true;
            Controller = ControllerStyle.Game;

            MovementSpeed = 50.0f;

            View = Matrix4x4.Identity;
            _lastSwipe = Vector2.Zero;
            // Reset();
            _pitch = 0.0f;
            _yaw = 0.0f;
            ComputeYawPitchRoll();
            UpdateView();
        }

        public void OnUpdate()
        {
            if (!Enabled) return;

            var ts = Global.Clock.Frame;
            _lastSwipe = Vector2.Zero;
            switch (Controller)
            {
                case ControllerStyle.Maya:
                {
                    if (Input.IsKeyPressed(KeyCode.LAlt))
                    {
                        _lastSwipe = Input.MouseSwipe * ts.AsSeconds() * 0.7f;

                        //if ( Input.IsMouseButtonPressed(SE_BUTTON_MIDDLE) )
                        //	MousePan(swipe);
                        //else if ( Input.IsMouseButtonPressed(SE_BUTTON_LEFT) )
                        //	MouseRotate(swipe);
                        //else if ( Input.IsMouseButtonPressed(SE_BUTTON_RIGHT) )
                        //	MouseZoom(swipe.y);
                    }

                    break;
                }
                case ControllerStyle.Game:
                {
                    var msScaled = MovementSpeed * ts.AsSeconds();
                    if (Input.IsKeyDown(KeyCode.W))
                    {
                        Position += Forward * msScaled;
                    }

                    if (Input.IsKeyDown(KeyCode.S))
                    {
                        Position -= Forward * msScaled;
                    }

                    if (Input.IsKeyDown(KeyCode.A))
                    {
                        Position -= Right * msScaled;
                    }

                    if (Input.IsKeyDown(KeyCode.D))
                    {
                        Position += Right * msScaled;
                    }

                    if (Input.IsKeyDown(KeyCode.E))
                    {
                        Position = new Vector3(Position.X, Position.Y + msScaled, Position.Z);
                    }

                    if (Input.IsKeyDown(KeyCode.Q))
                    {
                        Position = new Vector3(Position.X, Position.Y - msScaled, Position.Z);
                    }

                    MovementSpeed = Input.IsKeyDown(KeyCode.LShift) ? 20.0f : 10.0f;

                    if (Input.IsMouseButtonDown(MouseButtonCode.Right))
                    {
                        _lastSwipe = Input.MouseSwipe * ts.AsSeconds() * 0.3f;
                        ComputeYawPitchRoll();
                    }

                    break;
                }
                default:
                    throw new ArgumentOutOfRangeException();
            }

            if (_wantNewView)
            {
                UpdateView();
            }
        }

        public void OnGuiRender()
        {
            ImGui.Begin("Editor Camera");
            // Gui.BeginPropertyGrid();
            // Gui.Property("Exposure", GetExposure(), 0.0f, 100.0f, 0.1f, Gui.PropertyFlag.Drag);
            // Gui.EndPropertyGrid();
            ImGui.End();
        }


        public void Reset()
        {
            const float fPI = (float) Math.PI;
            Position = new Vector3(0.0f, 10.0f, 10.0f);
            Yaw = 4 * fPI / 3;
            Pitch = -fPI / 5.0f;
            _wantNewView = true;
        }

        public bool Enabled { get; set; }
        public Matrix4x4 View { get; private set; }
        public Matrix4x4 ViewProjection => Projection * View;

        public Vector3 Position
        {
            get => _position;
            set
            {
                _position = value;
                _wantNewView = true;
            }
        }

        public Vector3 Rotation
        {
            get => _rotation;
            set
            {
                _rotation = value;
                _wantNewView = true;
            }
        }

        public float Pitch
        {
            get => _pitch;
            set
            {
                _pitch = value;
                _wantNewView = true;
            }
        }

        public float Yaw
        {
            get => _yaw;
            set
            {
                _yaw = value;
                _wantNewView = true;
            }
        }

        public Vector3 Up { get; private set; }
        public Vector3 Right { get; private set; }
        public Vector3 Forward { get; private set; }

        public ControllerStyle Controller { get; set; }
        public float MovementSpeed { get; set; }

        private void OnMouseScroll(MouseWheelScrollEvent mwse)
        {
        }

        private void ComputeYawPitchRoll()
        {
            Yaw += _lastSwipe.X;
            Pitch -= _lastSwipe.Y;
            const float fPI = (float) Math.PI;
            Pitch = GenUtils.Clamped(Pitch, -fPI / 2.0f + 0.01f, fPI / 2.0f - 0.01f);
            _wantNewView = true;
        }

        private void UpdateView()
        {
            var front = new Vector3
            {
                X = (float) (Math.Cos(_yaw) * Math.Cos(_pitch)),
                Y = (float) Math.Sin(_pitch),
                Z = (float) (Math.Sin(_yaw) * Math.Cos(_pitch))
            };
            Forward = Vector3.Normalize(front);

            Right = Vector3.Normalize(Vector3.Cross(Forward, _worldUp));
            Up = Vector3.Normalize(Vector3.Cross(Right, Forward));

            View = Matrix4x4.CreateLookAt(Position, Position + Forward, Up);

            _wantNewView = false;
        }
    };
}