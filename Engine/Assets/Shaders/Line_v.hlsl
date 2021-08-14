#include "Common_i.hlsl"

struct VsInput
{
	float3 Position : Position;
	float4 Color : Color;
};

struct VsOutput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
};

VsOutput main(VsInput input)
{
	VsOutput output;

	output.Position = mul(float4(input.Position, 1.0), Mvp);
	output.Color = input.Color;

	return output;
}
