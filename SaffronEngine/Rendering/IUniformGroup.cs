namespace SaffronEngine.Rendering
{
    public interface IUniformGroup
    {
        void SubmitConst();
        void SubmitPerFrame();
        void SubmitPerDraw();

        void Dispose();
    }
}