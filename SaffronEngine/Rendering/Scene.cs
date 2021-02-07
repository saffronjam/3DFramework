using System.Numerics;
using SaffronEngine.Common;

namespace SaffronEngine.Rendering
{
    public class Scene
    {
        public Entity.Registry EntityRegistry { get; private set; }
        private readonly SceneRenderer _sceneRendererHandle;
        private readonly Camera _camera;
        public Camera ActiveCamera => _camera;

        public Scene(SceneRenderer sceneRendererHandle)
        {
            EntityRegistry = new Entity.Registry();
            _sceneRendererHandle = sceneRendererHandle;
            _camera = new EditorCamera();
        }

        public void OnUpdate()
        {
            ((EditorCamera) _camera).OnUpdate();
        }

        public void OnRender()
        {
            _sceneRendererHandle.Camera = new SceneRenderer.CameraData(_camera.Projection, ((EditorCamera) _camera).View);
            
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