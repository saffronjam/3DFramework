#include "Common_i.hlsl"

struct VsInput
{
	float3 Position : Position;
	float4 Color : Color;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT main(VsInput input)
{
	VS_OUTPUT output;

	output.Position = mul(float4(input.Position, 1.0), Mvp);
	output.Color = input.Color;

	return output;
}
