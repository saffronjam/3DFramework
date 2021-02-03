using SaffronEngine.Common;
using Sandbox.Layers;

namespace Sandbox
{
    class App : Application
    {
        static void Main()
        {
            var app = new App("Sandbox", 1280, 720);
            app.Run();
        }

        public App(string title, uint width, uint height)
            : base(title, width, height)
        {
        }

        public override void OnInit()
        {
            PushLayer(new EditorLayer());
        }

        public override void OnUpdate()
        {
            
        }

        public override void OnGuiRender()
        {
        }
    }
}