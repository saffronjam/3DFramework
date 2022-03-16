#include "Shadow_i.hlsl"

struct VsInput
{
	float3 Position : Position;
	float3 Normal : Normal;
	float3 Tangent : Tangent;
	float3 Binormal : Binormal;
	float2 TexCoord : TexCoord;
};

struct VsOutput
{
	float4 Position : SV_POSITION;
};

VsOutput main(VsInput input)
{
	VsOutput output;

	float4 pos4 = float4(input.Position, 1.0);
	pos4 = mul(pos4, Model);
	output.Position = pos4;

	return output;
}
