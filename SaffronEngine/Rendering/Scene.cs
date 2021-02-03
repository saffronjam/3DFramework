using SaffronEngine.Common;

namespace SaffronEngine.Rendering
{
    public class Scene
    {
        public Entity.Registry EntityRegistry { get; private set; }

        public Scene()
        {
            EntityRegistry = new Entity.Registry();
        }

        public virtual void OnUpdate()
        {
                
        }
        
        
        
    }
}