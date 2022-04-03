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
	float4 PixPos : PixPos;
};

VsOutput main(VsInput input)
{
	VsOutput output;

	float4 pos4 = float4(input.Position, 1.0);
	output.Position = mul(pos4, Mvp);
	output.PixPos = mul(pos4, Model);

	return output;
}
