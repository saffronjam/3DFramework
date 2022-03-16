cbuffer Camera : register(b0)
{
    matrix ViewProj;
}

struct VsInput
{
	float3 Position : Position;
};

struct VsOutput
{
    float3 WorldPos : Position;
	float4 Position : SV_POSITION;
};

VsOutput main(VsInput input)
{
	VsOutput output;

    output.WorldPos = input.Position;

    output.Position = mul(float4(input.Position, 0.0f), ViewProj);
    output.Position.z = output.Position.w;

	return output;
}
