namespace SaffronEngine.Rendering
{
    public readonly struct ClearValues
    {
        public uint Rgba { get; }
        public float Depth { get; }
        public byte Stencil { get; }

        public ClearValues(uint rgba = 0x30303000, float depth = 1.0f, byte stencil = 0)
        {
            Rgba = rgba;
            Depth = depth;
            Stencil = stencil;
        }
    };
}