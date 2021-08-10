#include "Common_i.hlsl"

struct VS_INPUT
{
	float3 Position : SV_POSITION;
	float4 Color : COLOR0;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 pos4 = float4(input.Position, 1.0);
	pos4 = mul(pos4, Mvp);
	output.Position = pos4;
	output.Color = input.Color;

	return output;
}
