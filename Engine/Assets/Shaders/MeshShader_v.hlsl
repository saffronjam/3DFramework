#include "Common_i.hlsl"

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
	float3 WorldPosition : WorldPosition;

	float3 Normal : Normal;
	float2 TexCoord : TexCoord;
	float3 Tangent : Tangent;
	float3 Binormal : Binormal;

	float3 ViewPos : ViewPos;
	float3 LightPos[MAX_LIGHTS] : LightPos;

	float3x3 WorldNormals : WorldNormals;
};

VsOutput main(VsInput input)
{
	VsOutput output;

	output.Position = mul(float4(input.Position, 1.0f), Mvp);
	output.WorldPosition = (float3)mul(float4(input.Position, 1.0f), Model);

    output.Normal = mul((float3x3)Model, input.Normal);
	output.TexCoord = input.TexCoord;
	output.Tangent = input.Tangent;
	output.Binormal = input.Binormal;

	output.ViewPos = CameraPosition;

	output.WorldNormals = mul(float3x3(input.Tangent, input.Binormal, input.Normal), Model);


	if (nPointLights >= 0)
	{
		output.LightPos[0] = PointLights[0].Position;
	}
	if (nPointLights >= 2)
	{
		output.LightPos[1] = (float3)mul(float4(PointLights[1].Position, 1.0f), Model);
	}
	if (nPointLights >= 3)
	{
		output.LightPos[2] = (float3)mul(float4(PointLights[2].Position, 1.0f), Model);
	}
	if (nPointLights >= 4)
	{
		output.LightPos[3] = (float3)mul(float4(PointLights[3].Position, 1.0f), Model);
	}

	return output;
}
