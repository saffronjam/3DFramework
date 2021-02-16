using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Numerics;
using ImGuiNET;
using Renderer;
using SaffronEngine.Platform.Windows;
using SaffronEngine.Rendering;

namespace SaffronEngine.Common
{
    public class Gui
    {
        private static Gui _inst;
        private readonly Impl _impl;

        public Gui()
        {
            _inst = this;
            _impl = new Impl(SceneRenderer.ViewID.Gui);

            AddFont("C:/Windows/Fonts/segoeui.ttf", 8);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 12);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 14);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 20);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 22);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 24);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 28);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 32);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 56);
            AddFont("C:/Windows/Fonts/segoeui.ttf", 72);
        }

        public void Begin()
        {
            _impl.Begin();
        }

        public void End()
        {
            _impl.End();
        }


        public void Shutdown()
        {
            _impl.Shutdown();
        }

        public static IntPtr AddTexture(Texture texture)
        {
            return _inst._impl.AddTexture(texture);
        }

        public static void RemoveTexture(IntPtr textureId)
        {
            _inst._impl.RemoveTexture(textureId);
        }

        public static ImFontPtr AddFont(string path, int size)
        {
            return _inst._impl.AddFont(path, size);
        }

        public static void SetFontSize(int size)
        {
            _inst._impl.SetFont(size);
        }


        private class Impl
        {
            private readonly IntPtr _imguiContext;
            private readonly SceneRenderer.ViewID _bgfxId;

            private Texture FontAtlas;
            private readonly Dictionary<int, ImFontPtr> _fonts = new Dictionary<int, ImFontPtr>();
            private readonly Dictionary<IntPtr, Texture> _textures = new Dictionary<IntPtr, Texture>();

            private Program _imguiProgram;
            private Program _textureProgram;

            private VertexLayout _vertexLayout;
            private Uniform _texUniform;

            private bool _windowHasFocus = false;
            private readonly bool[] MousePressed = new bool[] {false, false, false};
            private Texture _fontTexture = null;
            private int[] MouseCursors = new int[(int) ImGuiMouseCursor.COUNT];

            public Impl(SceneRenderer.ViewID bgfxId)
            {
                _bgfxId = bgfxId;
                _imguiContext = ImGui.CreateContext();

                SetupFlags();
                SetupKeyMappings();
                SetupEventHandlers();
                SetupBuffers();
                SetupFonts();
            }

            public void Shutdown()
            {
                ImGui.DestroyContext(_imguiContext);
            }

            public void Begin()
            {
                var io = ImGui.GetIO();
                io.DisplaySize = Application.Instance.Window.Size;
                io.DeltaTime = Global.Clock.Frame.AsSeconds();

                if (_windowHasFocus)
                {
                    if (io.WantSetMousePos)
                    {
                        var newMousePos = new Point((int) io.MousePos.X, (int) io.MousePos.Y);
                        User32.SetCursorPos(newMousePos.X, newMousePos.Y);
                    }
                    else
                    {
                        io.MousePos = Input.MousePosition;
                    }

                    for (var i = 0; i < 3; i++)
                    {
                        io.MouseDown[i] = MousePressed[i] || Input.IsMouseButtonDown((MouseButtonCode) i);
                        MousePressed[i] = false;
                    }
                }

                // Update Ctrl, Shift, Alt, Super state
                io.KeyCtrl = io.KeysDown[(int) KeyCode.LControl] || io.KeysDown[(int) KeyCode.RControl];
                io.KeyAlt = io.KeysDown[(int) KeyCode.LAlt] || io.KeysDown[(int) KeyCode.RAlt];
                io.KeyShift = io.KeysDown[(int) KeyCode.LShift] || io.KeysDown[(int) KeyCode.RShift];
                io.KeySuper = io.KeysDown[(int) KeyCode.LSystem] || io.KeysDown[(int) KeyCode.RSystem];

                ImGui.NewFrame();
            }

            public void End()
            {
                Render();
            }

            private void Render()
            {
                ImGui.EndFrame();
                ImGui.Render();

                var io = ImGui.GetIO();
                var drawData = ImGui.GetDrawData();
                var width = io.DisplaySize.X;
                var height = io.DisplaySize.Y;

                Bgfx.SetViewName((ushort) _bgfxId, "ImGui");
                Bgfx.SetViewMode((ushort) _bgfxId, ViewMode.Default);

                unsafe
                {
                    var identityMatrix = Matrix4x4.Identity;
                    Bgfx.SetViewTransform((ushort) _bgfxId, null, &identityMatrix.M11);
                }

                Bgfx.SetViewRect((ushort) _bgfxId, 0, 0, (int) width, (int) height);

                for (var ii = 0; ii < drawData.CmdListsCount; ++ii)
                {
                    var drawList = drawData.CmdListsRange[ii];
                    var numVertices = drawList.VtxBuffer.Size;
                    var numIndices = drawList.IdxBuffer.Size;

                    Bgfx.AllocateTransientBuffers(numVertices, _vertexLayout, numIndices, out var tvb, out var tib);

                    unsafe
                    {
                        var vertices = tvb.Data;
                        Buffer.MemoryCopy(drawList.VtxBuffer.Data.ToPointer(), vertices.ToPointer(),
                            numVertices * sizeof(ImDrawVert), numVertices * sizeof(ImDrawVert));

                        var indices = tib.Data;
                        Buffer.MemoryCopy(drawList.IdxBuffer.Data.ToPointer(), indices.ToPointer(),
                            numIndices * sizeof(ushort), numIndices * sizeof(ushort));
                    }

                    var offset = 0;
                    for (var cmdIndex = 0; cmdIndex < drawList.CmdBuffer.Size; cmdIndex++)
                    {
                        var cmd = drawList.CmdBuffer[cmdIndex];

                        if (cmd.UserCallback != IntPtr.Zero)
                        {
                            throw new NotImplementedException();
                        }

                        if (0 != cmd.ElemCount)
                        {
                            var state = 0
                                        | RenderState.WriteRGB
                                        | RenderState.WriteA
                                        | RenderState.Multisampling
                                ;

                            var th = FontAtlas;
                            var program = _imguiProgram;

                            // Check if the TextureId is not the one of the FontAtlas
                            // Short: We give imgui a IntPtr to identify the texture. In c++ we can give the pointer to the texture, 
                            // but I've dont't know if thats possible to do in c#.
                            // My solution: Cast the hashcode of the texture to a IntPtr and use it as an identifier and store the IntPtrs and the Textures in a dictionary.
                            // See imgui textureID/texId for more information.
                            if (cmd.TextureId != (IntPtr) FontAtlas.GetHashCode())
                            {
                                // Bgfx sets the state dependent on the texture flags. Getting these flags is not implemented?, so I ignore the check.
                                state |= RenderState.BlendFunction(RenderState.BlendSourceAlpha,
                                    RenderState.BlendInverseSourceAlpha);

                                th = _textures[cmd.TextureId];
                                if (0 != th.MipLevels)
                                {
                                    float[] lodEnabled = {1.0f, 0.0f, 0.0f, 0.0f};
                                    var imageLodEnabled = new Uniform("u_params", UniformType.Vector4);
                                    unsafe
                                    {
                                        fixed (void* lodEnabledPtr = lodEnabled)
                                        {
                                            Bgfx.SetUniform(imageLodEnabled, lodEnabledPtr);
                                        }
                                    }

                                    // If I use the texture shader from bgfx, the images aren't drawn
                                    //program = _textureProgram;
                                }
                            }
                            else
                            {
                                state |= RenderState.BlendFunction(RenderState.BlendSourceAlpha,
                                    RenderState.BlendInverseSourceAlpha);
                            }

                            // I've splitted the declaration and the usage, so it's clearer
                            var x = (int) Math.Max(cmd.ClipRect.X, 0.0f);
                            var y = (int) Math.Max(cmd.ClipRect.Y, 0.0f);
                            var z = (int) Math.Min(cmd.ClipRect.Z, 65535.0f);
                            var w = (int) Math.Min(cmd.ClipRect.W, 65535.0f);
                            Bgfx.SetScissor(x, y, z - x, w - y);

                            Bgfx.SetRenderState(state);
                            Bgfx.SetTexture(0, _texUniform, th);
                            Bgfx.SetVertexBuffer(0, tvb, 0, numVertices);
                            Bgfx.SetIndexBuffer(tib, offset, (int) cmd.ElemCount);
                            Bgfx.Submit((ushort) _bgfxId, program);
                        }

                        offset += (int) cmd.ElemCount;
                    }
                }
            }

            public IntPtr AddTexture(Texture texture)
            {
                var textureId = (IntPtr) texture.GetHashCode();
                if (!_textures.ContainsKey(textureId))
                {
                    _textures.Add(textureId, texture);
                }

                return textureId;
            }

            public void RemoveTexture(IntPtr textureId)
            {
                if (_textures.ContainsKey(textureId))
                {
                    _textures.Remove(textureId);
                }
            }

            public ImFontPtr AddFont(string path, int size)
            {
                var exists = File.Exists(path);
                if (!exists)
                {
                    return new ImFontPtr();
                }

                var newFont = ImGui.GetIO().Fonts.AddFontFromFileTTF(path, size);
                _fonts.Add(size, newFont);
                return newFont;
            }

            public void SetFont(int size)
            {
            }

            private static ImFontPtr GetAppropriateFont(int size)
            {
                ImFontPtr candidate = null;
                var bestDiff = int.MaxValue;
                foreach (var (key, value) in _inst._impl._fonts)
                {
                    var diff = Math.Abs(key - size);
                    if (diff >= bestDiff) continue;
                    bestDiff = diff;
                    candidate = value;
                }

                return candidate;
            }

            private void SetupEventHandlers()
            {
                var window = Application.Instance.Window;

                window.MouseButtonPressed += OnMouseButtonPressed;
                window.MouseButtonReleased += OnMouseButtonReleased;
                window.MouseWheelScrolled += OnMouseWheelScrolled;
                window.KeyPressed += OnKeyPressed;
                window.KeyReleased += OnKeyReleased;
                window.TextEntered += OnTextEntered;
                window.GainedFocus += OnGainedFocus;
                window.LostFocus += OnLostFocus;
            }

            private void SetupBuffers()
            {
                _imguiProgram = ResourceLoader.LoadProgram("ocornut_imgui", "ocornut_imgui");
                _textureProgram = ResourceLoader.LoadProgram("imgui_image", "imgui_image");

                _vertexLayout = new VertexLayout();
                _vertexLayout.Begin()
                    .Add(VertexAttributeUsage.Position, 2, VertexAttributeType.Float)
                    .Add(VertexAttributeUsage.TexCoord0, 2, VertexAttributeType.Float)
                    .Add(VertexAttributeUsage.Color0, 4, VertexAttributeType.UInt8, true)
                    .End();

                _texUniform = new Uniform("s_tex", UniformType.Sampler);
            }

            private void SetupFonts()
            {
                var io = ImGui.GetIO();
                _fonts.Add(18, io.Fonts.AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 18));

                // Build and save the fontatlas
                unsafe
                {
                    io.Fonts.GetTexDataAsRGBA32(out byte* data, out var width, out var height, out var bytesPerPixel);

                    var atlas = Texture.Create2D(width, height, false, 1, TextureFormat.BGRA8, 0,
                        new MemoryBlock((IntPtr) data, width * height * bytesPerPixel));

                    FontAtlas = atlas;
                    _textures.Add((IntPtr) atlas.GetHashCode(), atlas);
                    io.Fonts.SetTexID((IntPtr) atlas.GetHashCode());
                }
            }

            private void SetupFlags()
            {
                var io = ImGui.GetIO();

                io.BackendFlags |= ImGuiBackendFlags.HasMouseCursors;
                io.BackendFlags |= ImGuiBackendFlags.HasSetMousePos;
                io.ConfigFlags |= ImGuiConfigFlags.NavEnableKeyboard; // Enable Keyboard Controls
                //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
                io.ConfigFlags |= ImGuiConfigFlags.DockingEnable; // Enable Docking
                // io.ConfigFlags |= ImGuiConfigFlags.ViewportsEnable; // Enable Multi-Viewport / Platform Windows
                //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
                //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
            }

            private void SetupKeyMappings()
            {
                var io = ImGui.GetIO();

                io.KeyMap[(int) ImGuiKey.Tab] = (int) KeyCode.Tab;
                io.KeyMap[(int) ImGuiKey.LeftArrow] = (int) KeyCode.Left;
                io.KeyMap[(int) ImGuiKey.RightArrow] = (int) KeyCode.Right;
                io.KeyMap[(int) ImGuiKey.UpArrow] = (int) KeyCode.Up;
                io.KeyMap[(int) ImGuiKey.DownArrow] = (int) KeyCode.Down;
                io.KeyMap[(int) ImGuiKey.PageUp] = (int) KeyCode.PageUp;
                io.KeyMap[(int) ImGuiKey.PageDown] = (int) KeyCode.PageDown;
                io.KeyMap[(int) ImGuiKey.Home] = (int) KeyCode.Home;
                io.KeyMap[(int) ImGuiKey.End] = (int) KeyCode.End;
                io.KeyMap[(int) ImGuiKey.Insert] = (int) KeyCode.Insert;
                io.KeyMap[(int) ImGuiKey.Delete] = (int) KeyCode.Delete;
                io.KeyMap[(int) ImGuiKey.Backspace] = (int) KeyCode.Backspace;
                io.KeyMap[(int) ImGuiKey.Space] = (int) KeyCode.Space;
                io.KeyMap[(int) ImGuiKey.Enter] = (int) KeyCode.Enter;
                io.KeyMap[(int) ImGuiKey.Escape] = (int) KeyCode.Escape;
                io.KeyMap[(int) ImGuiKey.A] = (int) KeyCode.A;
                io.KeyMap[(int) ImGuiKey.C] = (int) KeyCode.C;
                io.KeyMap[(int) ImGuiKey.V] = (int) KeyCode.V;
                io.KeyMap[(int) ImGuiKey.X] = (int) KeyCode.X;
                io.KeyMap[(int) ImGuiKey.Y] = (int) KeyCode.Y;
                io.KeyMap[(int) ImGuiKey.Z] = (int) KeyCode.Z;
            }

            private void OnMouseButtonPressed(object sender, MouseButtonEventArgs args)
            {
                var button = (int) args.Button;
                if (button >= 0 && button < 3)
                {
                    MousePressed[(int) args.Button] = true;
                }
            }

            private void OnMouseButtonReleased(object sender, MouseButtonEventArgs args)
            {
                var button = (int) args.Button;
                if (button >= 0 && button < 3)
                {
                    MousePressed[(int) args.Button] = false;
                }
            }

            private void OnMouseWheelScrolled(object sender, MouseWheelScrollEventArgs args)
            {
                var io = ImGui.GetIO();
                switch (args.Wheel)
                {
                    case MouseWheelCode.VerticalWheel:
                    case MouseWheelCode.HorizontalWheel when io.KeyShift:
                        io.MouseWheel += args.Delta;
                        break;
                    case MouseWheelCode.HorizontalWheel:
                        io.MouseWheelH += args.Delta;
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
            }

            private void OnKeyPressed(object sender, KeyEventArgs args)
            {
                if (args.Code == KeyCode.Unknown) return;
                var io = ImGui.GetIO();
                io.KeysDown[(int) args.Code] = true;
            }

            private void OnKeyReleased(object sender, KeyEventArgs args)
            {
                if (args.Code == KeyCode.Unknown) return;
                var io = ImGui.GetIO();
                io.KeysDown[(int) args.Code] = false;
            }

            private void OnTextEntered(object sender, TextEventArgs args)
            {
                var io = ImGui.GetIO();
                // Don't handle the event for unprintable characters
                if (args.Unicode[0] < ' ' || args.Unicode[0] == 127)
                {
                    return;
                }

                io.AddInputCharacter(args.Unicode[0]);
            }

            private void OnGainedFocus(object sender, EventArgs args)
            {
                _windowHasFocus = true;
            }

            private void OnLostFocus(object sender, EventArgs args)
            {
                _windowHasFocus = false;
            }
        }
    }
}