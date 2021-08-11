#include "Common_i.hlsl"

struct VsInput
{
	float3 Position : Position;
	float3 Normal : Normal;
	float3 Tangent : Tangent;
	float3 Binormal : Binormal;
	float2 TexCoord : TexCoord;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT main(VsInput input)
{
	VS_OUTPUT output;

	float4 pos4 = float4(input.Position, 1.0);
	pos4 = mul(pos4, Mvp);
	output.Position = pos4;
	output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return output;
}
