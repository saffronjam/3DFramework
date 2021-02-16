using System.Numerics;
using ImGuiNET;
using SaffronEngine.Common;

namespace SaffronEngine.Rendering
{
    public class SceneSettings
    {
        public LightType LightType;
        public SmDepthImpl SmDepthImpl;
        public SmImpl SmImpl;
        public float SpotOuterAngle;
        public float SpotInnerAngle;
        public float FovXAdjust;
        public float FovYAdjust;
        public float CoverageSpotL;
        public float SplitDistribution;
        public int  NoSplits;
        public bool UpdateLights;
        public bool UpdateScene;
        public bool DrawDepthBuffer;
        public bool ShowSmCoverage;
        public bool StencilPack;
        public bool Stabilize;
    }

    public class Scene
    {
        public Entity.Registry EntityRegistry { get; private set; }
        private readonly SceneRenderer _sceneRendererHandle;
        private readonly Camera _camera;
        public Camera ActiveCamera => _camera;
        public SceneSettings Settings { get; set; }

        public Scene(SceneRenderer sceneRendererHandle)
        {
            EntityRegistry = new Entity.Registry();
            _sceneRendererHandle = sceneRendererHandle;
            _camera = new EditorCamera();
            Settings = new SceneSettings
            {
                LightType = LightType.Spot,
                SmDepthImpl = SmDepthImpl.InvZ,
                SmImpl = SmImpl.VSM,
                SpotOuterAngle = 60.0f,
                SpotInnerAngle = 30.0f,
                FovXAdjust = 0.0f,
                FovYAdjust = 0.0f,
                CoverageSpotL = 120.0f,
                SplitDistribution = 0.6f,
                NoSplits = 4,
                UpdateLights = true,
                UpdateScene = true,
                DrawDepthBuffer = false,
                ShowSmCoverage = false,
                StencilPack = true,
                Stabilize = true
            };



        }

        public void OnUpdate()
        {
            ((EditorCamera) _camera).OnUpdate();
        }

        public void OnRender()
        {
            _sceneRendererHandle.Camera =
                new SceneRenderer.CameraData(_camera.Projection, ((EditorCamera) _camera).View);

            ImGui.Begin("Scene renderer");
            _sceneRendererHandle.Begin(this);

            var group = EntityRegistry.AllWith(typeof(Component.Mesh), typeof(Component.Transform));
            foreach (var entity in group)
            {
                var transformComponent = entity.GetComponent<Component.Transform>();
                var meshComponent = entity.GetComponent<Component.Mesh>();

                SceneRenderer.Sumbit(meshComponent.Handle, transformComponent.Matrix);
            }

            // auto cameraGroup = m_EntityRegistry.group<CameraComponent>(entt::get<TransformComponent>);
            // for ( const auto 
            // &entityHandle : cameraGroup )
            // {
            //     auto[cameraComponent, transformComponent] =
            //         cameraGroup.get<CameraComponent, TransformComponent>(entityHandle);
            //     if (cameraComponent.DrawMesh && cameraComponent.CameraMesh)
            //     {
            //         m_SelectedEntity == entityHandle
            //             ? SceneRenderer::SubmitSelectedMesh(cameraComponent.CameraMesh, transformComponent)
            //             : SceneRenderer::SubmitMesh(cameraComponent.CameraMesh, transformComponent);
            //     }
            //
            //     if (cameraComponent.DrawFrustum)
            //     {
            //         cameraComponent.Camera->RenderFrustum(transformComponent.Transform);
            //     }
            // }

            _sceneRendererHandle.End();
            ImGui.End();
        }

        public void OnResize(uint width, uint height)
        {
            _camera.ViewportSize = new Vector2(width, height);
        }

        public Entity CreateEntity()
        {
            return EntityRegistry.Add(new Entity());
        }
    }
}