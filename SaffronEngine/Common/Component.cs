namespace SaffronEngine.Common
{
    public interface ISaffronComponent
    {
    }

    public class Component
    {
        public class Mesh : ISaffronComponent
        {
            public Mesh Handle { get; private set; }
        }
    }
}