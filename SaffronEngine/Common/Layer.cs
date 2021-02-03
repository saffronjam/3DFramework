namespace SaffronEngine.Common
{
    public interface Layer
    {
        public void OnAttach(Batch batch);

        public void OnDetach();

        public void OnUpdate();

        public void OnGuiRender();
    }
}